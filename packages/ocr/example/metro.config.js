const { getDefaultConfig } = require('expo/metro-config')
const path = require('node:path')

const appRoot = __dirname
const monorepoRoot = path.resolve(__dirname, '../..')

const config = getDefaultConfig(appRoot)

config.watchFolders = [monorepoRoot]
config.resolver.nodeModulesPaths = [path.resolve(appRoot, 'node_modules'), path.resolve(monorepoRoot, 'node_modules')]

config.resolver.unstable_enablePackageExports = true

module.exports = config
