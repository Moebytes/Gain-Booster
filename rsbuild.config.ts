import {defineConfig} from "@rsbuild/core"
import {pluginReact} from "@rsbuild/plugin-react"
import {pluginSass} from "@rsbuild/plugin-sass"

export default defineConfig({
    plugins: [pluginReact(), pluginSass()],
    dev: {
        hmr: true,
        writeToDisk: true
    },
    html: {
        template: "./index.html"
    },
    source: {
        entry: {
            index: "./index.tsx"
        }
    },
    output: {
        filenameHash: false
    }
})