project('blive_m3u8', 'c')

deps = [dependency('libcurl')]

src = files(
  'main.c',
  'parser/m3u8_url.c',
  'utils/log.c',
  'utils/utils.c',
  'api/m3u8.c'
  )

executable('blive_m3u8', src, dependencies: [deps])
