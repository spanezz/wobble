wobble_sources = [
  'string.cc',
  'subprocess.cc',
  'sys.cc',
  'term.cc',
  'tests.cc',
  'string-test.cc',
  'subprocess-test.cc',
  'sys-test.cc',
  'testrunner.cc',
  'tests-main.cc',
  'tests-test.cc',
]

test_wobble = executable('wobble-test', wobble_sources, implicit_include_directories: false)

runtest = find_program('../run-test')

test('wobble', runtest, args: [test_wobble])
