import fs from "node:fs";
import OpenAI from "openai";

export type Word = { text: string; start: number; end: number };

const openai = new OpenAI();

export async function transcribeClip(clipPath: string): Promise<Word[]> {
  const resp = await openai.audio.transcriptions.create({
    file: fs.createReadStream(clipPath),
    model: "whisper-1",
    response_format: "verbose_json",
    timestamp_granularities: ["word"],
  });

  const words = (resp as unknown as { words?: Array<{ word: string; start: number; end: number }> }).words ?? [];
  return words.map((w) => ({ text: w.word, start: w.start, end: w.end }));
}
