project('blive_m3u8', 'c')

deps = [dependency('libcurl'), dependency('yyjson')]

src = files(
  'main.c',
  'parser/m3u8_url.c',
  'parser/config.c',
  'utils/log.c',
  'utils/utils.c',
  'api/m3u8.c',
  'api/segment.c'
  )

executable('blive_m3u8', src, dependencies: [deps])
