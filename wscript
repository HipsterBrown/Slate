import json
import os

from waflib.Configure import conf

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def build(ctx):
    ctx.load('pebble_sdk')

    js_target = ctx.concat_javascript(js_path='src/js')

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf='{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
        target=app_elf)

        if build_worker:
            worker_elf='{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'),
            target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})

    ctx.set_group('bundle')
    ctx.pbl_bundle(binaries=binaries, js=js_target)

@conf
def concat_javascript(ctx, js_path=None):
    js_nodes = (ctx.path.ant_glob(js_path + '/**/*.js'))

    if not js_nodes:
        return []

    REQUIRE_TEMPLATE = ("__loader.define({relpath}, {lineno}, " +
                        "function(exports, module, require) {{\n{body}\n}});")
    # Wrap commonJS modules with __require define function
    def loader_transport(source, lineno):
        return REQUIRE_TEMPLATE.format(
            relpath=json.dumps(source['relpath']),
            lineno=lineno,
            body=source['body'])

    def concat_javascript_task(task):
        sources = []
        for node in task.inputs:
            relpath = os.path.relpath(node.abspath(), js_path)
            with open(node.abspath(), 'r') as f:
                body = f.read()
                if (relpath == 'loader/loader.js'):
                    # insert the loader directly
                    sources.insert(0, body)
                else:
                    # insert the loader directly
                    sources.append({ 'relpath': relpath, 'body': body })

        # Last thing is to load the app
        sources.append('__loader.require("main");')

        with open(task.outputs[0].abspath(), 'w') as f:
            lineno = 1
            for source in sources:
                if type(source) is dict:
                    body = loader_transport(source, lineno)
                else:
                    body = source
                f.write(body + '\n')
                lineno += body.count('\n') + 1

    js_target = ctx.path.make_node('build/src/js/pebble-js-app.js')

    ctx(rule=concat_javascript_task,
        source=js_nodes,
        target=js_target)

    return js_target
