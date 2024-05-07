#! /bin/sh
# file: utilNVMeCli/utilNVMeCli.sh

ScriptPath=$(pwd)
ShUnit2Path=${ScriptPath}/../shunit2/
BinPath=${ScriptPath}/../../../bin

testIdentify() {
  result=`cd ${BinPath} && ./utilNVMeCli identify`
  echo ${result}
  assertContains \
  	"Issue identify command failed" \
  	"${result}" "Successful"
}

testIdentifyFileOutput() {
  result=`cd ${BinPath} && ./utilNVMeCli identify --output=${ScriptPath}/output.bin`
  echo ${result}
  assertContains \
  	"Issue identify command failed" \
  	"${result}" "Successful"
}

testAnotherJsonfile() {
  result=`cd ${BinPath} && ./utilNVMeCli --jsonfile=${ScriptPath}/testJsonfile.json fake --debug=1`
  echo ${result}
  assertContains \
  	"Issue identify command failed" \
  	"${result}" "Successful"	
}

# Load shUnit2.
. ${ShUnit2Path}/shunit2
