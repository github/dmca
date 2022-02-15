/**
 * Returns true if the date portion of the filepath is in the form of YYYY-MM-DD.
 * @param {string} filepath
 * @returns {boolean}
 */
 export function isFilepathDateValid(filepath) {
  const filename = filepath.split("/").pop();
  const dateStringInFilename = filename.match(/\d{4}-\d{2}-\d{2}/);
  return dateStringInFilename !== null;
}

/**
 * Returns true if the filepath indicates that the file resides in the correct folder.
 * E.g., if the filepath is "2022/01/2022-01-01-abc.md", then the date portion
 * appears to be in the correct folder.
 * However, if the file predates the Children of the Great Foldering
 * (https://github.com/github/dmca-internal/issues/4301), then the file should be
 * in the year folder.
 * @param {string} filepath
 * @returns {boolean}
 */
export function isFileInCorrectFolder(filepath) {
  const dateStringInFilename = filepath.match(
    /(?<fileYear>\d{4})-(?<fileMonth>\d{2})-(?<fileDay>\d{2})/
  );

  if (dateStringInFilename === null) {
    return false;
  }

  const { fileYear, fileMonth, fileDay } = dateStringInFilename.groups;
  const fileDate = new Date(fileYear, fileMonth - 1, fileDay);
  const dateWhenMonthlyFoldersWereImplemented = new Date(2018, 9, 10);
  const [directoryYear, directoryMonth] = filepath.split("/");

  return fileDate < dateWhenMonthlyFoldersWereImplemented
    ? directoryYear === fileYear
    : directoryYear === fileYear && directoryMonth === fileMonth;
}

/**
 * Returns one of the following DMCA notice categories based on the filepath:
 * - takedown
 * - takedown-retraction
 * - takedown-reversal
 * - counternotice
 * - counternotice-retraction
 * - counternotice-reversal
 * @param {string} filepath
 * @returns {string}
 */
export function getNoticeCategory(filepath) {
  const filename = filepath.split("/").pop();
  let noticeType =
    [
      ...filename.matchAll(
        /(CounterNotice-Retraction|CounterNotice-Reversal|Counter-Retraction|Retraction|CounterNotice|Reversal)/gi
      ),
    ][0]?.[1]?.toLowerCase() || "takedown";

  if (noticeType === "counter-retraction") {
    noticeType = "counternotice-retraction";
  }

  return noticeType;
}
