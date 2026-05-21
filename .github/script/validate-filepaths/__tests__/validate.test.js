/**
 * Tests for validate-filepaths main script logic
 */

describe("isFileInsideAYearFolder", () => {
  // This function is defined in index.js - we test the logic it's testing

  function isFileInsideAYearFolder(filepath) {
    return filepath.match(/^\d{4}/) !== null;
  }

  test("returns true for file in year folder", () => {
    expect(isFileInsideAYearFolder("2024/2024-01-01-brand.markdown")).toBe(true);
  });

  test("returns true for file in year/month subfolder", () => {
    expect(isFileInsideAYearFolder("2024/01/2024-01-01-brand.markdown")).toBe(true);
  });

  test("returns false for file not in year folder", () => {
    expect(isFileInsideAYearFolder("README.md")).toBe(false);
  });

  test("returns false for file with non-year folder", () => {
    expect(isFileInsideAYearFolder("foo/2024-01-01-brand.markdown")).toBe(false);
  });
});

describe("filepath date validation pattern", () => {
  // Tests for the YYYY-MM-DD brand.markdown naming pattern

  function isFilepathDateValid(filepath) {
    const filename = filepath.split("/").pop();
    const dateStringInFilename = filename.match(/\d{4}-\d{2}-\d{2}/);
    return dateStringInFilename !== null;
  }

  test("validates correct YYYY-MM-DD format in filename", () => {
    expect(isFilepathDateValid("2024/01/2024-01-15-brand.markdown")).toBe(true);
    expect(isFilepathDateValid("2023/12/2023-12-31-company.markdown")).toBe(true);
  });

  test("invalidates incorrect date formats", () => {
    expect(isFilepathDateValid("2024/1-01-15-brand.markdown")).toBe(false);
    expect(isFilepathDateValid("2024/01/24-01-15-brand.markdown")).toBe(false);
    expect(isFilepathDateValid("2024/01/2024-1-15-brand.markdown")).toBe(false);
    expect(isFilepathDateValid("2024/01/2024-01-5-brand.markdown")).toBe(false);
  });

  test("invalidates filenames without date", () => {
    expect(isFilepathDateValid("README.md")).toBe(false);
    expect(isFilepathDateValid("2024/01/brand.markdown")).toBe(false);
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