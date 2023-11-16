project('logic', 'cpp', version: '0.0.1')

sources = [
  'logic/scanner.cc',
  'logic/parser.cc'
]

cpp_args = [
  '-std=c++2b',
  '-Wpedantic',
  '-fno-exceptions',
  '-fno-rtti'
]

liblogic = static_library(
  'logic',
  sources: sources,
  cpp_args: cpp_args,
  include_directories: ['logic']
)

executable(
  'logic',
  sources: 'logic/main.cc',
  cpp_args: cpp_args,
  link_with: liblogic
)

gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_dep')
gmock_dep = gtest.get_variable('gmock_dep')

test_sources = [
  'tests/testRunner.cc',
  'tests/testScanner.cc',
  'tests/testParser.cc',
]

test_runner = executable(
  'logic-tester',
  sources: test_sources,
  dependencies: [
    gtest_dep,
    gmock_dep,
  ],
  cpp_args: cpp_args,
  link_with: liblogic,
  include_directories: ['logic'],
)

test('logic-tester', test_runner)
