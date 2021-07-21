const fs = require("fs-extra")
const path = require("path");
const repsep = pathstr => pathstr.replace(/[/\\]/g, path.sep);
const projectDir = process.cwd();
const package_json = require(repsep(`${projectDir}/package.json`));

const NwBuilder = require("nw-builder");
const nw = new NwBuilder(package_json.build);

const log4js = require("log4js");
log4js.configure({
  appenders: {
    console: {
      type: "console",
    },
    system: {
      type: "dateFile",
      filename: repsep(`${projectDir}/logs/release.log`),
      pattern: "yyyy_MM_dd-hh_mm_ss",
    },
  },
  categories: {
    default: {
      appenders: [
        "console",
        "system",
      ],
      level: "all",
    },
  },
});
const logger = log4js.getLogger("release");

nw.on("log", log => logger.info(log));
fs.removeSync(repsep(`${projectDir}/dist`));
nw.build().then(() => {
  logger.info("All done!");
}).catch(error => {
  logger.error(error);
});
