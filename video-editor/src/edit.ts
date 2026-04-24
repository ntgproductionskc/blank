import { spawn } from "node:child_process";
import fs from "node:fs/promises";
import os from "node:os";
import path from "node:path";
import type { EditPlan, Segment } from "./highlights.ts";

const COLOR_GRADES: Record<string, string> = {
  punchy:
    "eq=saturation=1.25:contrast=1.12,curves=preset=increase_contrast",
  cinematic:
    "curves=r='0/0 0.5/0.55 1/1':b='0/0.05 1/0.95',eq=saturation=0.9:contrast=1.05",
  warm: "colorbalance=rs=0.10:gs=0.02:bs=-0.08,eq=saturation=1.1",
  moody: "eq=saturation=0.7:contrast=1.2:brightness=-0.05",
};

const ASPECTS: Record<string, { w: number; h: number }> = {
  "9:16": { w: 1080, h: 1920 },
  "16:9": { w: 1920, h: 1080 },
  "1:1": { w: 1080, h: 1080 },
};

type RenderOpts = {
  style: string;
  aspect: string;
};

export async function assembleVideo(
  plan: EditPlan,
  output: string,
  opts: RenderOpts,
): Promise<void> {
  const dims = ASPECTS[opts.aspect] ?? ASPECTS["9:16"]!;
  const grade = COLOR_GRADES[opts.style] ?? COLOR_GRADES.punchy!;

  const tmp = await fs.mkdtemp(path.join(os.tmpdir(), "video-editor-"));
  try {
    const segFiles: string[] = [];
    for (let i = 0; i < plan.segments.length; i++) {
      const seg = plan.segments[i]!;
      const outFile = path.join(
        tmp,
        `seg_${String(i).padStart(3, "0")}.mp4`,
      );
      process.stdout.write(
        `  [${i + 1}/${plan.segments.length}] ${seg.emphasis}: "${seg.caption}"... `,
      );
      await renderSegment(seg, outFile, { ...dims, grade });
      segFiles.push(outFile);
      console.log("ok");
    }

    const listFile = path.join(tmp, "concat.txt");
    await fs.writeFile(
      listFile,
      segFiles.map((f) => `file '${f.replaceAll("'", "'\\''")}'`).join("\n"),
    );

    await fs.mkdir(path.dirname(path.resolve(output)), { recursive: true });

    await ffmpeg([
      "-y",
      "-f",
      "concat",
      "-safe",
      "0",
      "-i",
      listFile,
      "-c:v",
      "libx264",
      "-preset",
      "medium",
      "-crf",
      "20",
      "-c:a",
      "aac",
      "-b:a",
      "192k",
      "-pix_fmt",
      "yuv420p",
      "-movflags",
      "+faststart",
      output,
    ]);
  } finally {
    await fs.rm(tmp, { recursive: true, force: true });
  }
}

async function renderSegment(
  seg: Segment,
  outFile: string,
  opts: { w: number; h: number; grade: string },
): Promise<void> {
  const duration = Math.max(0.5, seg.end - seg.start);
  const punch = seg.emphasis === "hook" || seg.emphasis === "punchline";
  const captionText = escapeDrawText(seg.caption);
  const fontSize = Math.round(opts.w * 0.06);
  const borderW = Math.max(2, Math.round(fontSize * 0.08));

  const captionFilter =
    `drawtext=text='${captionText}'` +
    `:fontcolor=white` +
    `:fontsize=${fontSize}` +
    `:borderw=${borderW}` +
    `:bordercolor=black` +
    `:x=(w-text_w)/2` +
    `:y=h-h/4` +
    `:alpha='if(lt(t,0.3),t/0.3,1)'`;

  const baseScale =
    `scale=${opts.w}:${opts.h}:force_original_aspect_ratio=increase,` +
    `crop=${opts.w}:${opts.h}`;

  const zoom = punch
    ? `zoompan=z='min(1.0+0.08*on/${Math.round(duration * 30)},1.08)':` +
      `d=1:s=${opts.w}x${opts.h}:fps=30,setsar=1`
    : null;

  const vf = [baseScale, opts.grade, zoom, captionFilter]
    .filter((s): s is string => !!s)
    .join(",");

  const af =
    `afade=t=in:st=0:d=0.15,` +
    `afade=t=out:st=${(duration - 0.15).toFixed(3)}:d=0.15`;

  await ffmpeg([
    "-y",
    "-ss",
    seg.start.toFixed(3),
    "-to",
    seg.end.toFixed(3),
    "-i",
    seg.clip,
    "-vf",
    vf,
    "-af",
    af,
    "-r",
    "30",
    "-c:v",
    "libx264",
    "-preset",
    "medium",
    "-crf",
    "20",
    "-c:a",
    "aac",
    "-b:a",
    "192k",
    "-pix_fmt",
    "yuv420p",
    outFile,
  ]);
}

function escapeDrawText(s: string): string {
  return s
    .replaceAll("\\", "\\\\")
    .replaceAll(":", "\\:")
    .replaceAll("'", "’")
    .replaceAll("%", "\\%");
}

function ffmpeg(args: string[]): Promise<void> {
  return new Promise((resolve, reject) => {
    const proc = spawn("ffmpeg", ["-hide_banner", "-loglevel", "error", ...args], {
      stdio: ["ignore", "inherit", "inherit"],
    });
    proc.on("error", (err) =>
      reject(
        new Error(
          `Failed to spawn ffmpeg (is it installed and on PATH?): ${err.message}`,
        ),
      ),
    );
    proc.on("exit", (code) =>
      code === 0
        ? resolve()
        : reject(new Error(`ffmpeg exited with code ${code}`)),
    );
  });
}
