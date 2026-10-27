const markdownPunctuation = new Set("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

export function escapeMarkdown(text) {
  return [...text]
    .map((character) => {
      if (character === "@") {
        return "&#64;";
      }

      return markdownPunctuation.has(character) ? `\\${character}` : character;
    })
    .join("");
}
