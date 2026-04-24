#!/usr/bin/env node
import { Command } from "commander";
import { run } from "./pipeline.ts";

const program = new Command();

program
  .name("video-editor")
  .description(
    "Automated video editor — transcribes raw clips, picks highlights with Claude, burns captions, color grades, and renders a final cut.",
  )
  .requiredOption("-i, --input <dir>", "Directory of raw video clips")
  .requiredOption("-o, --output <file>", "Output video path (.mp4)")
  .option(
    "-s, --style <name>",
    "Style preset: ntg | hormozi-gold | hormozi-yellow | cinematic | warm | moody",
    "ntg",
  )
  .option(
    "-a, --aspect <ratio>",
    "Aspect ratio: 9:16 | 16:9 | 1:1",
    "9:16",
  )
  .option("-d, --duration <secs>", "Target final duration in seconds", "60")
  .option(
    "--brief <text>",
    "Optional creative brief / topic to steer Claude's highlight selection",
    "",
  )
  .action(async (opts) => {
    try {
      await run({
        input: opts.input,
        output: opts.output,
        style: opts.style,
        aspect: opts.aspect,
        targetSeconds: Number(opts.duration),
        brief: opts.brief,
      });
    } catch (err) {
      console.error(
        "\nError:",
        err instanceof Error ? err.message : String(err),
      );
      process.exit(1);
    }
  });

program.parseAsync();
