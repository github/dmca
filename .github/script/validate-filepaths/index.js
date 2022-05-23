import walkSync from "walk-sync";
import {
  isFileInCorrectFolder,
  isFilepathDateValid,
  getNoticeCategory,
} from "./utils/validators.js";
import _ from "lodash";
const { partition } = _;
import { setOutput } from "@actions/core";

const { CHANGED_FILES } = process.env;
const dmcaFilesChangedInPr = CHANGED_FILES.split("\n").filter(
  isFileInsideAYearFolder
);

const [prChangedFilesWithValidDates, prChangedFilesWithInvalidDates] =
  partition(dmcaFilesChangedInPr, isFilepathDateValid);
const [prChangedFilesinCorrectFolder, prChangedFilesNotInCorrectFolder] =
  partition(prChangedFilesWithValidDates, isFileInCorrectFolder);

let commentBody = `Thanks for the PR!
# Files changed in this PR
`;
let commentCopyForFilesChangedInThisPr =
  prChangedFilesWithInvalidDates.length ||
  prChangedFilesNotInCorrectFolder.length
    ? ` I've scanned the changed files and found some issues to double check:`
    : ` All DMCA notices changed in this PR appear to have valid YYYY-MM-DD dates and seem to be in the correct folder.`;

commentCopyForFilesChangedInThisPr += prChangedFilesWithInvalidDates.length
  ? `\n\n**Notices that don't contain a date that conforms to the YYYY-MM-DD format:**\n\n${prChangedFilesWithInvalidDates
      .map((file) => `- ${file}`)
      .join("\n")}`
  : "";

commentCopyForFilesChangedInThisPr += prChangedFilesNotInCorrectFolder.length
  ? `\n\n**Notices that might need to be moved to the correct year/month folder:**\n\n${prChangedFilesNotInCorrectFolder
      .map((file) => `- ${file}`)
      .join("\n")}`
  : "";

let commentCopyForCategoriesOfFilesChangedInThisPr = `\n\n**Notices appear to fall into the following categories:**
| Filepath | Category |
| --- | --- |
${dmcaFilesChangedInPr
  .map((filepath) => {
    const noticeCategory = getNoticeCategory(filepath);
    return "| " + filepath + " | " + noticeCategory + " |";
  })
  .join("\n")}
`;

commentBody += commentCopyForFilesChangedInThisPr;
commentBody += commentCopyForCategoriesOfFilesChangedInThisPr;

const filesInDmcaNoticeFolders = walkSync(".", { directories: false })
  .filter(isFileInsideAYearFolder)
  .filter((file) => {
    const isFileAmongChangedFiles = dmcaFilesChangedInPr.includes(file);
    return !isFileAmongChangedFiles; // filter out files that are in the changed files list
  });

const [filesWithValidDates, filesWithInvalidDates] = partition(
  filesInDmcaNoticeFolders,
  isFilepathDateValid
);
const [filesInCorrectFolder, filesNotInCorrectFolder] = partition(
  filesWithValidDates,
  isFileInCorrectFolder
);

let commentCopyForAllDmcaNoticesInRepo =
  filesWithInvalidDates.length || filesNotInCorrectFolder.length
    ? `\n# Other Files in Repo\nI've also scanned the rest of the repo and found some files to double check:`
    : `\n# Other Files in Repo\nThe other DMCA notices in the repo appear to have valid YYYY-MM-DD dates and seem to be in the correct folder.`;

const invalidDatesList = filesWithInvalidDates.length
  ? `\n\n**Notices that don't contain a date that conforms to the YYYY-MM-DD format:**\n\n${filesWithInvalidDates
      .map((file) => `- ${file}`)
      .join("\n")}`
  : "";

const incorrectFolderList = filesNotInCorrectFolder.length
  ? `\n\n**Notices that might need to be moved to the correct year/month folder:**\n\n${filesNotInCorrectFolder
      .map((file) => `- ${file}`)
      .join("\n")}`
  : "";

commentCopyForAllDmcaNoticesInRepo += invalidDatesList;
commentCopyForAllDmcaNoticesInRepo += incorrectFolderList;

commentBody += commentCopyForAllDmcaNoticesInRepo;

setOutput("COMMENT_BODY", commentBody);

/**
 * Returns true if the file is in a folder at the root directory of the repo of the form YYYY
 * @param {string} filepath
 * @returns {boolean}
 */
function isFileInsideAYearFolder(filepath) {
  return filepath.match(/^\d{4}/) !== null;
}
