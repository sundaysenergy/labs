<?php
/**
 * @file
 * Domains.module's API
 */

/**
 * Implements this hook to provide your controler for your domain provider.
 * 
 * Return array of domain providers, each element is keyed-array, with
 *  $element['class'] is name of class implemented DomainControllerInterface.
 */
function hook_domains_controller_info() {
  return array(
    'enom' => array(
      'class' => 'EnomDomainController',
    ),
  );
}
