<?php
/**
 * @file
 * Domains.module's API
 */

/**
 * Implements this hook to provide your controler for your domain provider.
 */
function hook_domains_controller_info() {
  return array(
    'enom' => array(
      'class' => 'EnomDomainController',
    ),
  );
}
