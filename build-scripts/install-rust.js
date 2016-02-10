var fs = require('fs');
var path = require('path');

var rustBuildDir = path.join('target','release');
var addonBuildDir = path.join('build','Release');
var result = fs.readdirSync(rustBuildDir)
.filter(function(filename){
  return fs.statSync(path.join(rustBuildDir,filename)).isFile()
})
.forEach(function(filename){
  fs.createReadStream(path.join(rustBuildDir,filename))
  .pipe(fs.createWriteStream(path.join(addonBuildDir,filename)));
});
