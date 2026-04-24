import Anthropic from "@anthropic-ai/sdk";
import type { Word } from "./transcribe.ts";

const client = new Anthropic();

export type Emphasis = "intro" | "hook" | "punchline" | "outro" | "moment";

export type Segment = {
  clip: string;
  start: number;
  end: number;
  emphasis: Emphasis;
  keywords: string[];
};

export type EditPlan = { segments: Segment[] };

export type HighlightOptions = {
  style: string;
  targetSeconds: number;
  aspect: string;
  brief: string;
};

const SYSTEM_PROMPT = `You are an expert short-form video editor in the style of Alex Hormozi / Iman Gadzhi: direct, punchy, no dead air, word-by-word captions with keyword highlights. You receive transcripts of raw video clips with word-level timestamps and select the tightest, highest-retention moments.

Rules:
- Each segment must be 2-8 seconds of near-continuous speech. Pick ranges where consecutive words are <0.3s apart. Skip breaths, ums, and silences by choosing tight start/end times.
- Prioritize hooks (opening lines that stop the scroll), punchlines, concrete numbers, bold claims, and emotional beats.
- The first segment must be a strong hook — curiosity, contrarian claim, or specific promise. No throat-clearing.
- For each segment, return 1-4 keywords from what was actually said — the highest-charge words in that segment (the noun, number, or verb that carries the meaning). These will be highlighted in brand gold. Keywords must appear verbatim in the spoken words within the segment range. Use lowercase.
- Emphasis "hook" and "punchline" trigger a zoom effect; use them sparingly for the highest-energy lines.
- Timestamps must come from the word timestamps provided. Round to 2 decimals.
- Total duration of all segments should be close to the target (+/- 20%).
- Return exactly one edit plan via the emit_edit_plan tool.`;

export async function pickHighlights(
  transcripts: { clip: string; words: Word[] }[],
  opts: HighlightOptions,
): Promise<EditPlan> {
  const transcriptBlock = transcripts
    .map((t) => {
      const body = t.words
        .map((w) => `[${w.start.toFixed(2)}-${w.end.toFixed(2)}] ${w.text}`)
        .join(" ");
      return `=== ${t.clip} ===\n${body || "(no speech detected)"}`;
    })
    .join("\n\n");

  const userMessage = [
    `Target duration: ${opts.targetSeconds}s`,
    `Style: ${opts.style}`,
    `Aspect ratio: ${opts.aspect}`,
    opts.brief ? `Creative brief: ${opts.brief}` : null,
    "",
    "Transcripts:",
    transcriptBlock,
  ]
    .filter(Boolean)
    .join("\n");

  const response = await client.messages.create({
    model: "claude-opus-4-7",
    max_tokens: 16000,
    thinking: { type: "adaptive" },
    output_config: { effort: "high" },
    system: [
      {
        type: "text",
        text: SYSTEM_PROMPT,
        cache_control: { type: "ephemeral" },
      },
    ],
    tools: [
      {
        name: "emit_edit_plan",
        description:
          "Emit the final edit plan as an ordered list of segments to stitch together.",
        input_schema: {
          type: "object",
          properties: {
            segments: {
              type: "array",
              description: "Ordered list of clip segments.",
              items: {
                type: "object",
                properties: {
                  clip: {
                    type: "string",
                    description:
                      "Full clip path exactly as given in the === headers.",
                  },
                  start: {
                    type: "number",
                    description:
                      "Start time in seconds (from word timestamps).",
                  },
                  end: {
                    type: "number",
                    description:
                      "End time in seconds (from word timestamps).",
                  },
                  emphasis: {
                    type: "string",
                    enum: ["intro", "hook", "punchline", "outro", "moment"],
                  },
                  keywords: {
                    type: "array",
                    description:
                      "1-4 verbatim lowercase words spoken inside this segment to highlight in gold.",
                    items: { type: "string" },
                    minItems: 1,
                    maxItems: 4,
                  },
                },
                required: ["clip", "start", "end", "emphasis", "keywords"],
              },
            },
          },
          required: ["segments"],
        },
      },
    ],
    tool_choice: { type: "tool", name: "emit_edit_plan" },
    messages: [{ role: "user", content: userMessage }],
  });

  const toolUse = response.content.find(
    (b): b is Anthropic.ToolUseBlock => b.type === "tool_use",
  );
  if (!toolUse) {
    throw new Error("Claude did not return an edit plan.");
  }

  const plan = toolUse.input as EditPlan;
  if (!plan.segments?.length) {
    throw new Error("Edit plan has no segments.");
  }

  const validClips = new Set(transcripts.map((t) => t.clip));
  for (const seg of plan.segments) {
    if (!validClips.has(seg.clip)) {
      throw new Error(`Segment references unknown clip: ${seg.clip}`);
    }
    if (seg.end <= seg.start) {
      throw new Error(
        `Segment has invalid time range: ${seg.start} -> ${seg.end}`,
      );
    }
  }

  return plan;
}
