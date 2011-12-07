<?php

/**
 * @file
 * Default rule configurations for DynECT DNS.
 */

/**
 * Implements hook_default_rules_configuration().
 */

function wsclient_dynect_default_rules_configuration() {
  $items = array();
  $items['rules_create_zone'] = entity_import('rules_config', '{ "rules_create_zone" : {
    "LABEL" : "Create zone",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "dns" ],
    "REQUIRES" : [ "wsclient_dynect", "domains" ],
    "ON" : [ "domain_purchase" ],
    "DO" : [ { "wsclient_dynect_add_zone" : { "domain" : [ "domain" ] } } ]
  }
}');
  return $items;
}