project('logic', 'cpp', version: '0.0.1')

sources = [
  'logic/parsing/scanner.cc',
  'logic/parsing/parser.cc',
  'logic/parsing/sentence.cc',

  'logic/evaluation/evaluator.cc',
  'logic/evaluation/environment.cc',

  'logic/logic.cc',

  'logic/utils/table.cc',
  'logic/utils/utils.cc',
]

cpp_args = [
  '-std=c++2b',
  '-Wpedantic',
  '-fno-exceptions',
  '-fno-rtti',
  '-Wno-gnu-statement-expression'
]


gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_dep')
gmock_dep = gtest.get_variable('gmock_dep')
fmt = subproject('fmt')
fmt_dep = fmt.get_variable('fmt_dep')

liblogic = static_library(
  'logic',
  sources: sources,
  cpp_args: cpp_args,
  include_directories: ['logic'],
  dependencies: [
    fmt_dep
  ]
)

executable(
  'logic',
  sources: 'logic/main.cc',
  cpp_args: cpp_args,
  link_with: liblogic,
  dependencies: [
    fmt_dep
  ]
)

test_sources = [
  'tests/testRunner.cc',
  'tests/testScanner.cc',
  'tests/testParser.cc',
  'tests/testEvaluator.cc',
  'tests/testEnvironment.cc',

  'tests/printer.cc',
  'tests/reporter.cc',
]

test_runner = executable(
  'logic-tester',
  sources: test_sources,
  dependencies: [
    gtest_dep,
    gmock_dep,
    fmt_dep,
  ],
  cpp_args: cpp_args,
  link_with: liblogic,
  include_directories: ['logic'],
)

test('logic-tester', test_runner)
