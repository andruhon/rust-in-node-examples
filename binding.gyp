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
		"libraries": [
			"../target/debug/embed.dll.lib"
		]
	}]
}
