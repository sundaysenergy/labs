<?php

/**
 * @file
 * 
 * Commons functions for domain
 */

/**
 * Validate domain name
 * @return bool
 */
function domains_validate_name($domain, $allow_wildcard = TRUE, $warnings = FALSE) {
  $domain = strtolower($domain);

  // quick basic validate domain
/*
  $preg = '/^([a-z0-9]([-a-z0-9]*[a-z0-9])?\\.)+((a[cdefgilmnoqrstuwxz]|';
  if (preg_match($preg, $domain)) {
    drupal_set_message('valid domain name');
  }
*/

  // check for domain/tld separator
  $bits = explode('.', $domain, 2);

  if (empty($bits[1])) {
    if ($allow_wildcard) {
      $bits[1] = '*';
    }
    else {
      return FALSE;
    }
  }

  // SLD Must not start or end with '-'
  $len = strlen($bits[0]);
  if ($bits[0][0] == '-' || $bits[0][$len-1] == '-') {
    if ($warnings) {
      drupal_set_message(t('Domain names may not begin or end with a dash.'), 'error');
    }
    return FALSE;
  }

  // SLD Correct Lenght
  if ($len < 2 || $len > 63) {
    if ($warnings) {
      drupal_set_message(t('Domain names must be between 2 and 63 characters long.'), 'error');
    }
    return FALSE;
  }

  // if ($warnings) drupal_set_message(t('The domain name specified is malformed. Please check that it has the format: example.com'), 'error');

  // validate tld
  // @TODO cache this
/*
  $validtlds = enom_get_tlds();
  if (in_array($tld, $validtlds)==false) {
    if ($warnings) {
      drupal_set_message(t('The top-level domain specified (.@tld) is not available.', array('@tld' => $tld)), 'error');
    }
    return false;
  }
*/
  return TRUE;
}

/**
 * Check for domains avaiability
 * 
 * This function will list through all available domains controller
 * and run check() on each controller
 * @return array
 */
function domains_check($domains) {
  // Also accept domains array.
  if (!is_array($domains)) {
    $domains = array($domains);
  }

  $tlds = domains_get_tlds();
  $result = array();
  $api_call = array();

  if ($tlds) {
    foreach ($domains as $domain) {
      list($std, $tld) = explode(".", $domain, 2);

      // Just grab the first controller supported this domain
      // later we may support multiple controller per domain
      // so user can choose which provider they want to use.
      if (isset($tlds[$tld])) {
        $api_call[$tlds[$tld][0]][] = $domain;
      }
    }

    try {
      foreach ($api_call as $name => $check_domains) {
        foreach (domains_get_controller($name)->check($check_domains) as $domain => $avai) {
          if ($avai) {
            $result[$domain][] = $name;
          }
        }
      }
    }
    catch (DomainControllerException $ex) {
      _domains_handle_exception($ex);
      $result = FALSE;
    }
  }
  else {
    $result = FALSE;
  }

  return $result;
}

/**
 * Find a domain name in the controllers
 * @param string $domain
 * @return array
 */
function domains_find_domain($domain) {
  $result = array();
  $all_info = domains_get_controllers();

  try {
    foreach ($all_info as $name => $info) {
      $domain_info = domains_get_controller($name)->getInfo($domain);

      if (NULL !== $domain_info) {
        $result[$name] = $domain_info;
      }
    }
  }
  catch (DomainControllerException $ex) {
    _domains_handle_exception($ex);
    $result = FALSE;
  }

  return $result;
}

/**
 * Sync data between domain and provider
 * @param DomainEntity $domain
 * @param boolean $save Save the entity after refreshing
 * @return boolean
 */
function domains_refresh($domain, $save=TRUE) {
  try {
    // update domain entity
    $domain_info = domains_get_controller($domain->controller)->getInfo($domain->name);

    if ($domain_info) {
      $domain->expiration = $domain_info['expiration'];
      if ($save) {
        domains_domain_save($domain);
      }
      return TRUE;
    }
  }
  catch (DomainControllerException $ex) {
    _domains_handle_exception($ex);
  }

  return FALSE;
}

/**
 * Immediately renew a domain
 * @param DomainEntity $domain
 * @return boolean
 */
function domains_renew($domain, $years=1) {
  try {
    if ($domain->getController()->renew($domain->name, array('years' => $years))) {
      if (domains_refresh($domain)) {
        rules_invoke_event("domain_renew", $domain);
        return TRUE;
      }
    }
  }
  catch (DomainControllerException $ex) {
    _domains_handle_exception($ex);
  }

  return FALSE;
}

/**
 * Purchase a domain and create an associated entity with it
 * 
 * @return DomainEntity purchased domain entity or FALSE if there is error
 */
function domains_purchase($domain, $controller, $owner_uid=NULL) {
  if (NULL === $owner_uid) {
    global $user;
    $owner_uid = $user->uid;
  }

  $entity = (object)array(
    'uid' => $owner_uid,
    'name' => $domain,
    'controller' => $controller,
  );

  try {
    if (domains_get_controller($controller)->purchase($entity->name)) {
      if (domains_refresh($entity)) {
        rules_invoke_event('domain_purchase', $entity);
        return $entity;
      }
    }
  }
  catch (DomainControllerException $ex) {
    _domains_handle_exception($ex);
  }

  return FALSE;
}

/**
 * Get all available TLDs
 * @return array
 */
function domains_get_tlds($name_only=FALSE, $reset=FALSE) {
  $all_tlds =& drupal_static(__FUNCTION__);

  if ($all_tlds === NULL || $reset) {
    $all_tlds = array();

    $cache = cache_get('domains_tlds', 'cache');
    if (!$cache || $reset) {
      try {
        $all_info = domains_get_controllers();
        foreach ($all_info as $name => $info) {
          $controller = domains_get_controller($name);
  
          $tlds = (array)$controller->getTLDs();

          foreach ($tlds as $tld) {
            $all_tlds[$tld][] = $name;
          }
        }
      }
      catch (DomainControllerException $ex) {
        _domains_handle_exception($ex);
        $all_tlds = array();
      }

      // Do not cache if we have empty array.
      if (count($all_tlds)) {
        cache_set('domains_tlds', $all_tlds, 'cache', time() + 86400);
      }
    }
    else {
      $all_tlds = $cache->data;
    }
  }

  return $name_only ? array_keys($all_tlds) : $all_tlds;
}

/**
 * Get domain controller object
 * @param string $name
 * @return DomainControllerInterface
 */
function domains_get_controller($name) {
  $objects =& drupal_static(__FUNCTION__);

  // lazy constructor
  if (empty($objects[$name])) {
    $info = domains_get_controllers($name);

    if ($info) {
      $class = $info['class'];
      $objects[$name] = new $class;
    }
    else {
      return FALSE;
    }
  }

  return $objects[$name];
}

/**
 * Get domain controller info
 * @return array
 */
function domains_get_controllers($name=NULL) {
  $controllers =& drupal_static(__FUNCTION__);

  if ($controllers === NULL) {
    $controllers = array();

    foreach (module_implements('domains_controller_info') as $module) {
      $hook_function = "{$module}_domains_controller_info";

      $controllers += $hook_function();
    }
  }

  if ($name) {
    return isset($controllers[$name]) ? $controllers[$name] : NULL;
  }
  else {
    return $controllers;
  }
}

/**
 * Utility function to handle exceptions
 */
function _domains_handle_exception($exception) {
  // TODO: make options to silent these detail errors
  // TODO: and log them
  drupal_set_message($exception->getMessage(), 'error');
  
  if ($exception instanceof DomainControllerException) {
    foreach ($exception->getAdditionalErrors() as $error) {
      drupal_set_message($error, 'error');
    }
  }
}