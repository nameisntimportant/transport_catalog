#!/usr/bin/env python3

# Script for testing transport_catalog:
# 1. Runs "transport_catalog make_base", put input from make_base_[TEST_NAME] file to transport_catalog.
#    Transport_catalog calculates it's data, serialises it and stores to "PROTOBUF_BIN_FILES_PATH".
# 2. Runs "transport_catalog process_requests", put input from process_requests_[TEST_NAME] file to transport_catalog.
#    Transport_catalog deserialises data from "PROTOBUF_BIN_FILES_PATH" and process requests.
# 3. Compares output from 2-nd step with expected, wich is stored in expected_[TEST_NAME] file.

from sys import exit, argv
from os import listdir, mkdir, path, chdir
from subprocess import check_output, PIPE, CalledProcessError, run, STDOUT
from colorama import Fore, Style
from shutil import rmtree
from pathlib import Path

# Please, notice that if you want to change PROTOBUF_BIN_FILES_PATH, you should change it
# in testData/make_base_* and testData/process_requests_* files too.
PROTOBUF_BIN_FILES_PATH = '/tmp/transportcatalogdfs31235gsad324sct422/'

ACTUAL_OUTPUT_PATH = 'actual_output/'

MAKE_BASE_PARAM = 'make_base'
PROCESS_REQUESTS_PARAM = 'process_requests'

BASE_REQUESTS_FILE_NAME = 'base_requests.json'
STAT_REQUESTS_FILE_NAME = 'stat_requests.json'
EXPECTED_OUTPUT_FILE_NAME = 'expected.json'
ACTUAL_OUTPUT_FILE_NAME = 'actual.json'

MAKE_BASE_TIMEOUT = 3
PROCESS_REQUESTS_TIMEOUT = 1.25

TEST_DATA_PATH = 'test_data/'

INTEGRATION_TEST_MESSAGE_PREFIX = "integration test: "

def fail(returnCode, msg):
    print(INTEGRATION_TEST_MESSAGE_PREFIX + Fore.RED + 'ERROR ' + Style.RESET_ALL + msg)
    exit(returnCode)

def ok(msg):
    print(INTEGRATION_TEST_MESSAGE_PREFIX + Fore.GREEN + 'OK ' + Style.RESET_ALL + msg)

def runBin(binPath, parametr, inputFile, timeout):
    try:
        result = check_output([binPath, parametr], text=True, stdin=inputFile, stderr=STDOUT, timeout=timeout)
    except CalledProcessError as e:
        fail(2, 'command' + str(e.cmd) + ' input file "' + inputFile.name + '": error code: ' + str(e.returncode) +
             '.\nstdoutput + stderr: "' + str(e.output) + '"')
    return result

def ensurePathExistsAndIsEmpty(myPath):
    if path.exists(myPath):
        rmtree(myPath)
    mkdir(myPath)

def main():
    print("starting integration test")

    if (len(argv) != 2):
        fail(1, 'please pass path to transport_catalog binary file as a single parametr')
    transportCatalogBin = argv[1]

    absoluteTestDataPath = Path(TEST_DATA_PATH).absolute()
    testDataDirs = [x for x in absoluteTestDataPath.iterdir() if x.is_dir()]
    if (len(testDataDirs) == 0):
        fail(4, 'no test data dirs were found in' + absoluteTestDataPath)

    ensurePathExistsAndIsEmpty(PROTOBUF_BIN_FILES_PATH)
    ensurePathExistsAndIsEmpty(ACTUAL_OUTPUT_PATH)

    for testDataDir in testDataDirs:
        chdir(testDataDir)

        makeBaseInputFile = open(BASE_REQUESTS_FILE_NAME, 'r')
        runBin(transportCatalogBin, MAKE_BASE_PARAM, makeBaseInputFile, MAKE_BASE_TIMEOUT)

        processRequestsInputFile = open(STAT_REQUESTS_FILE_NAME, 'r')
        actualOutput = runBin(transportCatalogBin, PROCESS_REQUESTS_PARAM, processRequestsInputFile,
                              PROCESS_REQUESTS_TIMEOUT)

        actualOutputFile = open(ACTUAL_OUTPUT_FILE_NAME, 'w')
        actualOutputFile.write(actualOutput)
        actualOutputFile.close()

        filesDiff = run(['diff', '-w', EXPECTED_OUTPUT_FILE_NAME, ACTUAL_OUTPUT_FILE_NAME], stdout=PIPE, text=True)

        if filesDiff.stdout:
            fail(3, 'test data dir name: "' + str(testDataDir) + '", actual output is different from expected. "' +
                 ACTUAL_OUTPUT_FILE_NAME + '" is not equal with "' + EXPECTED_OUTPUT_FILE_NAME + '":\n' + filesDiff.stdout)
        else:
            ok('test in dir ' + str(testDataDir))
    ok('all tests')
    rmtree(PROTOBUF_BIN_FILES_PATH)
    return 0

if __name__ == '__main__':
    exit(main())
