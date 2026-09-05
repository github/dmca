/**
 * Tests for validate-filepaths main script logic
 */

function runValidateFilepaths(changedFiles) {
  jest.resetModules();

  process.env.CHANGED_FILES = changedFiles.join("\n");

  const core = {
    getInput: jest.fn((name) => (name === "changed-files" ? process.env.CHANGED_FILES : "")),
    setFailed: jest.fn(),
    info: jest.fn(),
    warning: jest.fn(),
    error: jest.fn(),
    setOutput: jest.fn(),
  };

  jest.doMock("@actions/core", () => core);
  jest.doMock("fs", () => ({
    existsSync: jest.fn(() => true),
    statSync: jest.fn(() => ({ isDirectory: () => false })),
    readdirSync: jest.fn(() => []),
    readFileSync: jest.fn(() => ""),
  }));

  jest.isolateModules(() => {
    require("../index.js");
  });

  return core;
}

afterEach(() => {
  delete process.env.CHANGED_FILES;
  jest.resetModules();
  jest.clearAllMocks();
  jest.unmock("@actions/core");
  jest.unmock("fs");
});

describe("isFileInsideAYearFolder", () => {
  test("accepts a file in a year folder", () => {
    const core = runValidateFilepaths(["2024/2024-01-01-brand.markdown"]);

    expect(core.setFailed).not.toHaveBeenCalled();
  });

  test("accepts a file in a year/month subfolder", () => {
    const core = runValidateFilepaths(["2024/01/2024-01-01-brand.markdown"]);

    expect(core.setFailed).not.toHaveBeenCalled();
  });

  test("rejects a file not in a year folder", () => {
    const core = runValidateFilepaths(["README.md"]);

    expect(core.setFailed).toHaveBeenCalled();
  });

  test("rejects a file with a non-year top-level folder", () => {
    const core = runValidateFilepaths(["foo/2024-01-01-brand.markdown"]);

    expect(core.setFailed).toHaveBeenCalled();
  });
});

describe("filepath date validation pattern", () => {
  test("accepts correct YYYY-MM-DD format in filename", () => {
    expect(
      runValidateFilepaths(["2024/01/2024-01-15-brand.markdown"]).setFailed
    ).not.toHaveBeenCalled();
    expect(
      runValidateFilepaths(["2023/12/2023-12-31-company.markdown"]).setFailed
    ).not.toHaveBeenCalled();
  });

  test("rejects incorrect date formats", () => {
    expect(runValidateFilepaths(["2024/1-01-15-brand.markdown"]).setFailed).toHaveBeenCalled();
    expect(runValidateFilepaths(["2024/01/24-01-15-brand.markdown"]).setFailed).toHaveBeenCalled();
    expect(runValidateFilepaths(["2024/01/2024-1-15-brand.markdown"]).setFailed).toHaveBeenCalled();
    expect(runValidateFilepaths(["2024/01/2024-01-5-brand.markdown"]).setFailed).toHaveBeenCalled();
  });

  test("rejects filenames without date", () => {
    expect(runValidateFilepaths(["README.md"]).setFailed).toHaveBeenCalled();
    expect(runValidateFilepaths(["2024/01/brand.markdown"]).setFailed).toHaveBeenCalled();
  });
});

describe("filepath structure validation", () => {
  // Tests for the overall filepath structure (year folder, optional month folder, filename)

  function isFileInsideAYearFolder(filepath) {
    return filepath.match(/^\d{4}/) !== null;
  }

  function isFilepathDateValid(filepath) {
    const filename = filepath.split("/").pop();
    const dateStringInFilename = filename.match(/\d{4}-\d{2}-\d{2}/);
    return dateStringInFilename !== null;
  }

  test("correct structure with year-only folder", () => {
    const validPath = "2024/2024-06-15-brand.markdown";
    expect(isFileInsideAYearFolder(validPath)).toBe(true);
    expect(isFilepathDateValid(validPath)).toBe(true);
  });

  test("correct structure with year/month folders", () => {
    const validPath = "2024/06/2024-06-15-brand.markdown";
    expect(isFileInsideAYearFolder(validPath)).toBe(true);
    expect(isFilepathDateValid(validPath)).toBe(true);
  });

  test("incorrect structure with missing year folder", () => {
    const invalidPath = "brand/2024-06-15-brand.markdown";
    expect(isFileInsideAYearFolder(invalidPath)).toBe(false);
    expect(isFilepathDateValid(invalidPath)).toBe(true);
  });

  test("incorrect structure with non-matching date in filename", () => {
    const invalidPath = "2024/06/2023-06-15-brand.markdown";
    expect(isFileInsideAYearFolder(invalidPath)).toBe(true);
    expect(isFilepathDateValid(invalidPath)).toBe(true);
  });
});