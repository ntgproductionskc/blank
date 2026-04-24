import fs from "node:fs/promises";
import path from "node:path";
import { transcribeClip, type Word } from "./transcribe.ts";
import { pickHighlights } from "./highlights.ts";
import { assembleVideo } from "./edit.ts";

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

  console.log(
    `Selected ${plan.segments.length} segment(s). Rendering to ${opts.output}...`,
  );
  await assembleVideo(plan, opts.output, {
    style: opts.style,
    aspect: opts.aspect,
  });

  console.log("Done.");
}
