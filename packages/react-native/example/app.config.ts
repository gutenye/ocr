const withEntitlementsPlist = require('@expo/config-plugins').withEntitlementsPlist

const withRemoveiOSNotificationEntitlement = (config) => {
  return withEntitlementsPlist(config, (mod) => {
    mod.modResults['aps-environment'] = undefined
    return mod
  })
}

type Dict = { [key: string]: any }

export default ({ config }: Dict) => {
  return {
    ...config,
    plugins: [...(config.plugins ?? []), [withRemoveiOSNotificationEntitlement]],
  }
}
