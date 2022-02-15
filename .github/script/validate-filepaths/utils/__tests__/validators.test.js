import {
  getNoticeCategory,
  isFileInCorrectFolder,
  isFilepathDateValid,
} from "../validators";

describe("getNoticeCategory", () => {
  it("should return takedown if the filepath does not match any of the regexes", () => {
    const filepath = "2022/01/2022-01-01-abc.md";
    expect(getNoticeCategory(filepath)).toBe("takedown");
  });

  it("should return `counternotice-retraction` if the filepath contains `Counter-Retraction`", () => {
    const filepath = "2022/01/2022-01-01-abc-Counter-Retraction.md";
    expect(getNoticeCategory(filepath)).toBe("counternotice-retraction");
  });

  it("should return `counternotice-retraction` if the filepath contains `CounterNotice-Retraction`", () => {
    const filepath = "2022/01/2022-01-01-abc-CounterNotice-Retraction.md";
    expect(getNoticeCategory(filepath)).toBe("counternotice-retraction");
  });

  it("should return `counternotice-reversal` if the filepath contains `CounterNotice-Reversal`", () => {
    const filepath = "2022/01/2022-01-01-abc-CounterNotice-Reversal.md";
    expect(getNoticeCategory(filepath)).toBe("counternotice-reversal");
  });

  it("should return `counternotice` if the filepath contains `CounterNotice`", () => {
    const filepath = "2022/01/2022-01-01-abc-CounterNotice.md";
    expect(getNoticeCategory(filepath)).toBe("counternotice");
  });

  it("should return `retraction` if the filepath contains `Retraction`", () => {
    const filepath = "2022/01/2022-01-01-abc-Retraction.md";
    expect(getNoticeCategory(filepath)).toBe("retraction");
  });

  it("should return `reversal` if the filepath contains `Reversal`", () => {
    const filepath = "2022/01/2022-01-01-abc-Reversal.md";
    expect(getNoticeCategory(filepath)).toBe("reversal");
  });
});

describe("isFileInCorrectFolder", () => {
  it("should return true if the file is in the correct year/month folder", () => {
    const filepath = "2022/01/2022-01-01-abc.md";
    expect(isFileInCorrectFolder(filepath)).toBe(true);
  });

  it("should return false if the filepath is not in the correct year/month folder", () => {
    const filepath = "2022/02/2022-01-01-abc.md";
    expect(isFileInCorrectFolder(filepath)).toBe(false);
  });

  it("should return true if the file is in the correct year folder for notices that predate October 10, 2018", () => {
    const filepath = "2018/2018-01-01-abc.md";
    expect(isFileInCorrectFolder(filepath)).toBe(true);
  });

  it("should return false if the filepath is not in the correct year folder for notices that predate October 10, 2018", () => {
    const filepath = "2017/2018-01-01-abc.md";
    expect(isFileInCorrectFolder(filepath)).toBe(false);
  });
});

describe("isFilepathDateValid", () => {
  it("should return true if the filepath contains a valid date", () => {
    const filepath = "2022/01/2022-01-01-abc.md";
    expect(isFilepathDateValid(filepath)).toBe(true);
  });

  it("should return false if the filepath does not contain a valid date", () => {
    const filepath = "2022/01/2022-1-01-abc.md";
    expect(isFilepathDateValid(filepath)).toBe(false);
  });

  it("should return false if the filepath does not contain a valid date", () => {
    const filepath = "2022/01/2022-01-1-abc.md";
    expect(isFilepathDateValid(filepath)).toBe(false);
  });

  it("should return false if the filepath does not contain a valid date", () => {
    const filepath = "2022/01/202-01-01-abc.md";
    expect(isFilepathDateValid(filepath)).toBe(false);
  });
});
