import fs from "node:fs/promises";
import path from "node:path";
import { transcribeClip, type Word } from "./transcribe.ts";
import { pickHighlights, type Segment } from "./highlights.ts";
import { assembleVideo, type RenderSegment } from "./edit.ts";

export type RunOptions = {
  input: string;
  output: string;
  style: string;
  aspect: string;
  targetSeconds: number;
  brief: string;
};

const VIDEO_EXT = /\.(mp4|mov|mkv|webm|m4v)$/i;

export async function run(opts: RunOptions): Promise<void> {
  const entries = await fs.readdir(opts.input);
  const clips = entries
    .filter((f) => VIDEO_EXT.test(f))
    .map((f) => path.join(opts.input, f))
    .sort();

  if (clips.length === 0) {
    throw new Error(`No video clips found in ${opts.input}`);
  }

  console.log(`Found ${clips.length} clip(s). Transcribing...`);
  const transcripts: { clip: string; words: Word[] }[] = [];
  for (const clip of clips) {
    process.stdout.write(`  ${path.basename(clip)}... `);
    const words = await transcribeClip(clip);
    transcripts.push({ clip, words });
    console.log(`${words.length} words`);
  }

  console.log("Picking highlights with Claude (Opus 4.7)...");
  const plan = await pickHighlights(transcripts, {
    style: opts.style,
    targetSeconds: opts.targetSeconds,
    aspect: opts.aspect,
    brief: opts.brief,
  });

  const wordsByClip = new Map(transcripts.map((t) => [t.clip, t.words]));
  const renderSegments: RenderSegment[] = plan.segments.map((seg) => ({
    ...seg,
    words: sliceWords(wordsByClip.get(seg.clip) ?? [], seg),
  }));

  console.log(
    `Selected ${renderSegments.length} segment(s). Rendering to ${opts.output}...`,
  );
  await assembleVideo(renderSegments, opts.output, {
    style: opts.style,
    aspect: opts.aspect,
  });

  console.log("Done.");
}

function sliceWords(words: Word[], seg: Segment): Word[] {
  return words
    .filter((w) => w.end > seg.start && w.start < seg.end)
    .map((w) => ({
      text: w.text,
      start: Math.max(0, w.start - seg.start),
      end: Math.min(seg.end - seg.start, w.end - seg.start),
    }));
}
