<?php

/**
 * @file
 * Default rule configurations for Domains.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function domains_default_rules_configuration() {
  $items = array();
  $items['rules_schedule_domain_expiration_reminder'] = entity_import('rules_config', '{ "rules_schedule_domain_expiration_reminder" : {
      "LABEL" : "Schedule domain expiration reminder",
      "PLUGIN" : "reaction rule",
      "TAGS" : [ "domains" ],
      "REQUIRES" : [ "rules", "rules_scheduler", "domains" ],
      "ON" : [ "domain_purchase", "domain_renew" ],
      "DO" : [
        { "schedule" : {
            "component" : "rules_send_domain_expiration_reminder",
            "date" : { "select" : "domain:expiration", "date_offset" : { "value" : -2592000 } },
            "identifier" : "domains-reminder:[domain:id]",
            "param_domain" : [ "domain" ]
          }
        }
      ]
    }
  }');
  $items['rules_send_domain_expiration_reminder'] = entity_import('rules_config', '{ "rules_send_domain_expiration_reminder" : {
      "LABEL" : "Send domain expiration reminder",
      "PLUGIN" : "action set",
      "TAGS" : [ "domains" ],
      "REQUIRES" : [ "rules" ],
      "USES VARIABLES" : { "domain" : { "label" : "Domain", "type" : "domain" } },
      "ACTION SET" : [
        { "mail" : {
            "to" : "[domain:creator:mail]",
            "subject" : "Domain Expiration reminder",
            "message" : "Your domain [domain:name] will expire at [domain:expiration]."
          }
        }
      ]
    }
  }');
  return $items;
}
