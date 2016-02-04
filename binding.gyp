{
	"targets": [{
		"target_name": "addon",
		"sources": ["src/addon.cc" ],
        'include_dirs': [
          '.',
        ],
        "include_dirs" : [
	 			"<!(node -e \"require('nan')\")"
		],
		"conditions": [
			['OS=="linux"', {
				"libraries": [
					"<(module_root_dir)/target/release/libembed.so"
				]
			}],
			['OS=="mac"', {
				"libraries": [
					"../target/release/libembed.dylib"
				]
			}],
			['OS=="win"', {
				"libraries": [
					"../target/release/embed.dll.lib"
				]
			}]
		]
	}]
}
