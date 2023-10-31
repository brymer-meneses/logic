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

