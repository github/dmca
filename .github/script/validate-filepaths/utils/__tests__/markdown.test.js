import { escapeMarkdown } from "../markdown";

describe("escapeMarkdown", () => {
  it("escapes Markdown syntax, table delimiters, and mentions", () => {
    expect(escapeMarkdown("2026/10/[notice](https://example.com) | @user")).toBe(
      "2026\\/10\\/\\[notice\\]\\(https\\:\\/\\/example\\.com\\) \\| &#64;user"
    );
  });
});
