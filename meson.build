project('wobble', 'cpp', version: '1.0', license : 'BSD', default_options: ['warning_level=everything', 'cpp_std=c++17'])

cpp = meson.get_compiler('cpp')

warning_control = [
  '-Wno-padded',
  '-Wno-abi-tag',
  '-Wswitch',
  '-Wno-switch-enum',
  '-Wno-effc++',
  #  '-Wsurprising', '-Wuseless-cast'
]

add_project_arguments(
  cpp.get_supported_arguments(warning_control),
  language : 'cpp')


subdir('wobble')
