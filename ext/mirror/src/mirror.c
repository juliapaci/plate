#include <gcc-plugin.h>
#include <plugin-version.h>

int plugin_is_GCC_compatible;

static struct plugin_info plugin_info = {
    .version = "0.1",
    .help = ""
};

// TODO: (plugin setup) should do a plugin_gcc_version

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version) {
    if(!plugin_default_version_check(version, &gcc_version))
        return 1;

    register_callback("mirror", PLUGIN_INFO, NULL, &plugin_info);

    printf("attached!\n");

    return 0;
}
