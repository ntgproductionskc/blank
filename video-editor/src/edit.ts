import { spawn } from "node:child_process";
import fs from "node:fs/promises";
import os from "node:os";
import path from "node:path";
import type { Segment } from "./highlights.ts";
import type { Word } from "./transcribe.ts";

export type RenderSegment = Segment & {
  words: Word[];
};

type StylePreset = {
  grade: string;
  textColor: string;
  keywordColor: string;
};

const PUNCHY =
  "eq=saturation=1.20:contrast=1.10,curves=preset=increase_contrast";
const CINEMATIC =
  "curves=r='0/0 0.5/0.55 1/1':b='0/0.05 1/0.95',eq=saturation=0.9:contrast=1.05";
const WARM = "colorbalance=rs=0.10:gs=0.02:bs=-0.08,eq=saturation=1.1";
const MOODY = "eq=saturation=0.7:contrast=1.2:brightness=-0.05";

const STYLES: Record<string, StylePreset> = {
  "hormozi-gold": {
    grade: PUNCHY,
    textColor: "0xFFFFFF",
    keywordColor: "0xFFD700",
  },
  "hormozi-yellow": {
    grade: PUNCHY,
    textColor: "0xFFFFFF",
    keywordColor: "0xFFEB3B",
  },
  cinematic: {
    grade: CINEMATIC,
    textColor: "0xFFFFFF",
    keywordColor: "0xFFD700",
  },
  warm: {
    grade: WARM,
    textColor: "0xFFFFFF",
    keywordColor: "0xE8B923",
  },
  moody: {
    grade: MOODY,
    textColor: "0xFFFFFF",
    keywordColor: "0xFFD700",
  },
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
  segments: RenderSegment[],
  output: string,
  opts: RenderOpts,
): Promise<void> {
  const dims = ASPECTS[opts.aspect] ?? ASPECTS["9:16"]!;
  const style = STYLES[opts.style] ?? STYLES["hormozi-gold"]!;

  const tmp = await fs.mkdtemp(path.join(os.tmpdir(), "video-editor-"));
  try {
    const segFiles: string[] = [];
    for (let i = 0; i < segments.length; i++) {
      const seg = segments[i]!;
      const outFile = path.join(
        tmp,
        `seg_${String(i).padStart(3, "0")}.mp4`,
      );
      const preview = seg.words
        .map((w) => w.text)
        .join(" ")
        .slice(0, 60);
      process.stdout.write(
        `  [${i + 1}/${segments.length}] ${seg.emphasis}: "${preview}"... `,
      );
      await renderSegment(seg, outFile, { ...dims, style });
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
  seg: RenderSegment,
  outFile: string,
  opts: { w: number; h: number; style: StylePreset },
): Promise<void> {
  const duration = Math.max(0.5, seg.end - seg.start);
  const punch = seg.emphasis === "hook" || seg.emphasis === "punchline";

  const keywordSet = new Set(
    seg.keywords.flatMap((k) =>
      k
        .toLowerCase()
        .split(/\s+/)
        .map((w) => w.replace(/[^a-z0-9']/g, ""))
        .filter(Boolean),
    ),
  );

  const fontSize = Math.round(opts.w * 0.078);
  const borderW = Math.max(3, Math.round(fontSize * 0.12));
  const yPos = "h*0.42-text_h/2";

  const wordFilters: string[] = [];
  for (let i = 0; i < seg.words.length; i++) {
    const w = seg.words[i]!;
    const nextStart =
      i + 1 < seg.words.length ? seg.words[i + 1]!.start : duration + 0.05;
    const displayStart = Math.max(0, w.start).toFixed(3);
    const displayEnd = Math.min(duration, nextStart).toFixed(3);

    const displayText = w.text
      .replace(/[^\p{L}\p{N}\s'-]/gu, "")
      .trim()
      .toUpperCase();
    if (!displayText) continue;

    const matchKey = w.text
      .toLowerCase()
      .replace(/[^a-z0-9']/g, "");
    const color = keywordSet.has(matchKey)
      ? opts.style.keywordColor
      : opts.style.textColor;

    wordFilters.push(
      `drawtext=text='${escapeDrawText(displayText)}'` +
        `:fontcolor=${color}` +
        `:fontsize=${fontSize}` +
        `:borderw=${borderW}` +
        `:bordercolor=black` +
        `:x=(w-text_w)/2` +
        `:y=${yPos}` +
        `:enable='between(t,${displayStart},${displayEnd})'`,
    );
  }

  const baseScale =
    `scale=${opts.w}:${opts.h}:force_original_aspect_ratio=increase,` +
    `crop=${opts.w}:${opts.h}`;

  const zoom = punch
    ? `zoompan=z='min(1.0+0.08*on/${Math.round(duration * 30)},1.08)':` +
      `d=1:s=${opts.w}x${opts.h}:fps=30,setsar=1`
    : null;

  const vf = [baseScale, opts.style.grade, zoom, ...wordFilters]
    .filter((s): s is string => !!s)
    .join(",");

  const af =
    `afade=t=in:st=0:d=0.1,` +
    `afade=t=out:st=${Math.max(0, duration - 0.1).toFixed(3)}:d=0.1`;

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
    const proc = spawn(
      "ffmpeg",
      ["-hide_banner", "-loglevel", "error", ...args],
      { stdio: ["ignore", "inherit", "inherit"] },
    );
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
