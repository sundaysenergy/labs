<?php

/**
 * @file
 * Function to domains.module pages.
 */

/**
 * Domain dashboard menu callback.
 */
function domains_dashboard_page($time = 0) {
  global $user;
  $own_domain_query = new EntityFieldQuery();
  $own_domain_query = $own_domain_query->entityCondition('entity_type', 'domains_domain')
    ->propertyCondition('uid', $user->uid);
  // Get expiring soon or expired
  if(!empty($time)) {
    if($time > 0) {
      // Get time expiring of domain <= $time
      $own_domain_query = $own_domain_query->propertyCondition('expiration', REQUEST_TIME + $time, '<=');
    }
    else {
      $own_domain_query = $own_domain_query->propertyCondition('expiration', REQUEST_TIME, '<');
    }
  }
  $own_domains = $own_domain_query->execute();
  $domains = array();
  if(isset($own_domains['domains_domain'])) {
    $results = domains_domain_load_multiple(array_keys($own_domains['domains_domain']));
    foreach ($results as $domain) {
      $domains[] = l($domain->name, "domain/" . $domain->id);
    }
  }

  return theme('item_list', array(
    'items' => $domains,
  ));
}

function domains_register_process_page() {
  // process callback, implement batch api for purchasing
  return 'TODO';
}

/**
 * Callback for domain page view.
 * 
 * @param type $domain
 * @param type $view_mode
 * @return type 
 */
function domains_domain_page_view($domain, $view_mode = 'full') {
  return entity_build_content('domains_domain', $domain, $view_mode);
}

/**
 * Add domain menu callback
 */
function domains_domain_add_callback() {
  $domain = (object) array (
    'id' => '',
    'type' => 'domains_domain',
    'note' => '',
  );

  return drupal_get_form('domain_add_form', $domain);
}

/**
 * Add domain form
 */
function domain_add_form($form, &$form_state, $domain) {
  global $user;

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
  );

  $form['uid'] = array(
    '#type' => 'value',
    '#default_value' => $user->uid,
  );
  
  $form['controller'] = array(
    '#type' => 'value',
    '#default_value' => '',
  );

  $form['expiration'] = array(
    '#type' => 'value',
    '#default_value' => '',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  field_attach_form('domains_domain', $domain, $form, $form_state);
  return $form;
}

/**
 * Add domain validate callback
 */
function domain_add_form_validate($form, &$form_state) {
  $form_state['values']['name'] = strtolower($form_state['values']['name']);
  $domain = $form_state['values']['name']; 

  // Validate domain name across system and provider
  if (!domains_validate_name($domain, FALSE)) {
    form_set_error('name', t('Please enter a valid domain like example.com'));
  }
  else {
    if (count(entity_load('domains_domain', FALSE, array('name' => $domain)))) {
      form_set_error('name', t('Domain @domain already existed in the system.', array('@domain' => $domain)));
    }
    else {
      $info = domains_find_domain($form_state['values']['name']);

      if ($info === FALSE) {
        form_set_error('name', t('Unable to validate your domain name.'));
      }
      else if (count($info)) {
        // Use the first provider for now.
        foreach ($info as $controller => $domain_info) {
          $form_state['values']['controller'] = $controller;
          $form_state['values']['expiration'] = $domain_info['expiration'];
          break;
        }
      }
      else {
        form_set_error('name', t('Domain @domain is not registered with any provider. Please use domain register form to regsiter new domain.', array('@domain' => $domain)));
      }
    }
  }

  $domain_submisttion = (object) $form_state['values'];
  field_attach_form_validate('domains_domain', $domain_submisttion, $form, $form_state);
}

/**
 * Domain add form callback 
 */
function domain_add_form_submit($form, &$form_state) {
  global $user;

  $domain = (object) $form_state['values'];
  field_attach_submit('domains_domain', $domain, $form, $form_state);
  domains_domain_save($domain);

  $form_state['redirect'] = "domain/{$domain->id}";
}

/**
 * Domains registration form.
 */
function domains_register_form($form, &$form_state) {
  $check_result = NULL;

  // check for available domains now
  if (isset($form_state['values']['domain'])) {
    // expand the domains if necessary
    $check_domain = $form_state['values']['domain'];
    
    if (strpos($check_domain, '.') === FALSE) {
      $domains = array();
      
      foreach (domains_get_tlds(TRUE) as $tld) {
        $domains[] = $check_domain . ".$tld";
      }
    }
    else {
      $domains = array($check_domain);
    }

    $check_result = domains_check($domains);
  }

  ctools_add_js('ajax-responder', 'ctools');

  $form['search'] = array(
    '#prefix' => '<div class="clearfix">',
    '#suffix' => '</div>',
  );

  $form['search']['domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain Name'),
  );

  // if we make it submit type, it won't work
  $form['search']['submit'] = array(
    '#type' => 'button',
    '#ajax' => array(
      'callback' => 'domains_register_form_check_ajax_submit',
      'wrapper' => 'results-div',
    ),
    '#value' => t('Check Your Domain'),
  );

  $form['results'] = array(
    '#title' => t("Search Results"),
    // The prefix/suffix provide the div that we're replacing, named by
    // #ajax['wrapper'] above.
    '#prefix' => '<div id="results-div">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
  );

  $form['results']['results_markup'] = array(
    '#type' => 'markup',
    '#markup' => t('Please search for a domain name.'),
  );

  // Populate domain data into form.
  if ($check_result !== NULL && $check_result !== FALSE) {
    $has_avai = FALSE;
    
    $form['results']['domains'] = array(
        '#type'   => 'checkboxes',
        '#check_result' => $check_result,
        '#title'  => t('Available Domains'),
        '#options' => array(),
    );

    // TODO: It would be better if we show unavailable domain as disabled checkbox.
    foreach ($check_result as $domain => $providers) {
      if ($providers) {
        $has_avai = TRUE;
        $form['results']['domains']['#options'][$domain] = $domain;
      }
    }

    if ($has_avai) {
      $form['results']['results_markup']['#markup'] = t('Please tick on the domain you would like to register then click purchase.');
      
      $form['results']['purchase'] = array(
        '#type' => 'button',
        '#ajax' => array(
          'callback' => 'domains_register_form_purchase_ajax_submit',
        ),
        '#value' => t('Purchase'),
      );
    }
    else {
      $form['results']['results_markup']['#markup'] = t('There is no available domains match your search criteria.');
      unset($form['results']['domains']);
    }
  }

  return $form;
}

/**
 * Custom validation form for the 'domain name check' page.
 *
 * @ingroup enom_register
 */
function domains_register_form_validate($form, &$form_state) {
  $domain = $form_state['values']['domain'];
  
  if (!domains_validate_name($domain, TRUE)) {
    form_set_error('domain', t('Please enter a valid domain like example.com'));
  }
}

/**
 * Select the 'box' element, change the markup in it, and return it as a
 * renderable array.
 *
 * @return renderable array (the box element)
 */
function domains_register_form_check_ajax_submit($form, &$form_state) {
  // In most cases, it is recomended that you put this logic in form generation
  // rather than the callback. Submit driven forms are an exception, because
  // you may not want to return the form at all.

  return $form['results'];
}

function domains_register_form_purchase_ajax_submit($form, &$form_state) {
  $commands = array();

  $selected_domains = array();
  
  foreach ($form_state['values']['domains'] as $domain => $checked) {
    if ($checked) {
      $selected_domains[] = $domain;
    }
  }

  if ($selected_domains) {
    ctools_include('ajax', 'ctools');

    $check_result = $form['results']['domains']['#check_result'];
    $succeed = FALSE;

    foreach ($selected_domains as $name) {
      $supported_controllers = $check_result[$name];

      // Let use the first one for now.
      if (count($supported_controllers)) {
        $domain = domains_purchase($name, $supported_controllers[0]);

        if ($domain) {
          $succeed = TRUE;
          break;
        }
      }
    }

    if (FALSE == $succeed) {
      $commands[] = ajax_command_alert(t('There were an error while we trying to purchase your selected domains. Please contact administrator for more information.'));
    }
    else {
      drupal_set_message(t('You have sucessfully purchase @count domain(s) to your account.', array('@count' => count($selected_domains))));
      $commands[] = ctools_ajax_command_redirect('domain');
    }
  }
  else {
    $commands[] = ajax_command_alert(t('Please tick on the domain you want to purchase.'));
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Domain renew form.
 */
function domains_renew_form($form, $form_state, $domain) {
  $form['domain'] = array(
    '#type' => 'value',
    '#value' => $domain,
  );
  
  $form['expiration'] = array(
    '#type' => 'item',
    '#title' => t('Expiration Date'),
    '#markup' => format_date($domain->expiration),
  );

  $form['options']['#tree'] = TRUE;
  $form['options']['years'] = array(
    '#title' => 'Number of Year',
    '#description' => t('Please select the number of years you want to renew'),
    '#default_value' => 1,
    '#type' => 'radios',
    '#options' => array(
      1 => t('1 year'),
      2 => t('2 years'),
      3 => t('3 years'),
      4 => t('4 years'),
      5 => t('5 years'),
    ),
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Renew'),
  );

  return $form;
}

/**
 * Domain renew submit callback.
 */
function domains_renew_form_submit($form, $form_state) {
  $domain = $form_state['values']['domain'];
  $years = $form_state['values']['options']['years'];
  
  if (domains_renew($domain, $years)) {
    drupal_set_message(t('You have renewed your domain successfully.'));
    drupal_goto('domain/' . $domain->id);
  }
  else {
    drupal_set_message(t('There was an error while renewing your domain. Please try again later.'), 'error');
  }
}

/**
 * Domain whois form
 */
function domains_whois_form($form, $form_state, $domain) {
  $whois = domains_whois($domain);
  if (!$whois) {
    drupal_set_message(t('Unable to retrive Whois information. Please retry later.'), 'error');
    return $form;
  }

  $form['#domain'] = $domain;
  $form['#whois'] = $whois;
  $form['#whois_types'] = array();

  // TODO: Implement copy from Registrant contact.

  // TODO: Data required by the Registry for some country codes. Use
  // GetExtAttributes to determine whether this TLD requires
  // extended attributes.
  foreach (domains_whois_types() as $type => $title) {
    if (isset($whois[$type])) {
      $whois_form = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#title' => $title,
        '#tree' => TRUE,
      );

      $whois_form['organization'] = array(
        '#type' => 'textfield',
        '#title' => t('Organization name'),
        '#default_value' => $whois[$type]['organization']
      );
      $whois_form['job_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Job title'),
        '#default_value' => $whois[$type]['job_title']
      );
      $whois_form['first_name'] = array(
        '#type' => 'textfield',
        '#title' => t('First name'),
        '#default_value' => $whois[$type]['first_name'],
        '#required' => TRUE,
      );
      $whois_form['last_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Last name'),
        '#default_value' => $whois[$type]['last_name'],
        '#required' => TRUE,
      );
      $whois_form['email'] = array(
        '#type' => 'textfield',
        '#title' => t('Email'),
        '#default_value' => $whois[$type]['email'],
        '#required' => TRUE,
      );
      $whois_form['address1'] = array(
        '#type' => 'textfield',
        '#title' => t('Address 1'),
        '#default_value' => $whois[$type]['address1'],
        '#required' => TRUE,
      );
      $whois_form['address2'] = array(
        '#type' => 'textfield',
        '#title' => t('Address 2'),
        '#default_value' => $whois[$type]['address2'],
      );
      $whois_form['phone'] = array(
        '#type' => 'textfield',
        '#title' => t('Phone'),
        '#default_value' => $whois[$type]['phone'],
        '#required' => TRUE,
      );
      $whois_form['phone_ext'] = array(
        '#type' => 'textfield',
        '#title' => t('Phone extension'),
        '#default_value' => $whois[$type]['phone_ext'],
      );
      $whois_form['fax'] = array(
        '#type' => 'textfield',
        '#title' => t('Fax'),
        '#default_value' => $whois[$type]['fax'],
      );
      $whois_form['country'] = array(
        '#type' => 'textfield',
        '#title' => t('Contry'),
        '#default_value' => $whois[$type]['country'],
        '#required' => TRUE,
      );
      $whois_form['state'] = array(
        '#type' => 'textfield',
        '#title' => t('State'),
        '#default_value' => $whois[$type]['state'],
        '#required' => TRUE,
      );
      $whois_form['city'] = array(
        '#type' => 'textfield',
        '#title' => t('City'),
        '#required' => TRUE,
        '#default_value' => $whois[$type]['city'],
      );
      $whois_form['postal_code'] = array(
        '#type' => 'textfield',
        '#title' => t('Postal code'),
        '#default_value' => $whois[$type]['postal_code'],
      );

      $form['#whois_types'][] = $type;
      $form[$type] = $whois_form;
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['save'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

function domains_whois_form_validate($form, $form_state) {
  foreach ($form['#whois_types'] as $type) {
    // TODO: do the validate
  }
}

function domains_whois_form_submit($form, $form_state) {
  $new_whois = array();
  foreach ($form['#whois_types'] as $type) {
    $info = $form_state['values'][$type];
    $new_whois[$type] = $info;
  }

  if (domains_update_whois($form['#domain'], $new_whois)) {
    drupal_set_message(t('Domain whois information has been updated.'));
  }
}

// Inport domain from registrar
function domains_domain_import_form($form, &$form_state) {
  ctools_add_js('ajax-responder', 'ctools');
  // Show all providers
  $providers = array();
  foreach(module_implements('domains_provider_info') as $module) {
    $providers = array_merge($providers, module_invoke($module, 'domains_provider_info'));
  }
  
  $form['provider'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Select your provider'),
    '#tree' => TRUE,
  );
  foreach($providers as $key => $provider) {
    $form['provider'][$key] = array(
      '#type' => 'checkbox',
      '#title' => $provider['label'],
    );
  }
  $form['next'] = array(
    '#type' => 'button',
    '#value' => t('Show domains can import'),
    '#ajax' => array(
      'callback' => 'domains_domain_import_list_ajax',
      'wrapper' => 'import-provider-wrapper',
    ),
    '#prefix' => '<div class="clearfix">',
    '#suffix' => '</div>',
  );
  $form['domains'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div id="import-provider-wrapper">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#title' => t("Avaiable domain"),
  );
  $form['domains']['markup'] = array(
    '#markup' => t('Please choose your provider to show avariable domain')
  );
  
  if(isset($form_state['values'])) {
    $values = $form_state['values'];
    $providers = array_filter($values['provider']);
    $header = array('name' => t('Name'));
    foreach($providers as $provider_name => $status) {
      $provider = domains_get_provider($provider_name);
      $info = $provider->getInfo();
      $form['domains'][$provider_name] = array(
        '#type' => 'fieldset',
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#title' => $info['label'],
        '#tree' => TRUE,
      );
      $domains = $provider->getDomains();
      $checkeds = array();
      $options = array();
      foreach($domains as $domain) {
        $domain_name = $provider->getDomainName($domain);
        $options[$domain_name] = array('name' => $domain_name);
//        $form['domains'][$provider_name][$domain_name] = array(
//          '#type' => 'checkbox',
//          '#title' => $domain_name,
//        );
        $checkeds[] = $domain_name;
      }
      // Get domain adrealy on our system
      $query = new EntityFieldQuery();
      $check = $query
        ->entityCondition('entity_type', 'domains_domain')
        ->propertyCondition('name', $checkeds, 'IN')
        ->execute();
      if(!empty($check['domains_domain'])) {
        $adrealyed = domains_domain_load_multiple(array_keys($check['domains_domain']));
        foreach($adrealyed as $domain) {
          //unset($form['domains'][$provider_name][$domain->name]);
          unset($options[$domain->name]);
        }
      }
      $form['domains'][$provider_name]['domains'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $options,
        '#empty' => t('No domain available.'),
      );
    }
    // Show import button
    $form['domains']['import'] = array(
      '#type' => 'submit',
      '#value' => t('Import'),
      '#ajax' => array(
        'callback' => 'domains_domain_import_domain_ajax',
        'wrapper' => 'import-provider-wrapper',
      ),
    );
    $form['domains']['markup']['#markup'] = '';
  }
  return $form;
}

function domains_domain_import_form_validate($form, &$form_state) {
  if(sizeof(array_filter($form_state['values']['provider'])) == 0) {
    form_set_error('provider', t('Please choose latest one provider'));
  }
}

function domains_domain_import_list_ajax($form, &$form_state) {
  return $form['domains'];
}

function domains_domain_import_domain_ajax($form, &$form_state) {
  $transfer = array();
  foreach($form_state['values']['domains'] as $provider => $values) {
    if(isset($values['domains'])) {
      $import = array_filter($values['domains']);
      if(sizeof($import) > 0) {
        $transfer[$provider] = $import;
      }
    }
  }
  ctools_include('ajax', 'ctools');
  ctools_include('object-cache', 'ctools');
  ctools_object_cache_set('import-domain', 'import-domain', $transfer);
  $commands = array(
    ctools_ajax_command_redirect('domain/import/process'),
  );
  return array('#type' => 'ajax', '#commands' => $commands);
}

function domains_domain_import_process_form($form) {
  ctools_include('object-cache', 'ctools');
  $transfer = ctools_object_cache_get('import-domain', 'import-domain');
  
  foreach($transfer as $provider_name => $domain_list) {
    $provider = domains_get_provider($provider_name);
    $info = $provider->getInfo();
    $form[$provider_name] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#title' => $info['label'],
      '#tree' => TRUE,      
    );
    $form[$provider_name]['domains'] = array(
      '#markup' => theme('item_list', array('items' => $domain_list)),
    );
  }
  
  return confirm_form($form,
    t('Are you sure to import thoose domain?'),
    'domain'
  );
}

function domains_domain_import_process_form_submit($form, &$form_state) {  
  $operations = array();
  $transfer = ctools_object_cache_get('import-domain', 'import-domain');
  foreach($transfer as $provider_name => $domain_list) {
    $provider = domains_get_provider($provider_name);
    foreach($domain_list as $domain) {
      $operations[] = array('_domains_domain_inport_process', array($domain, $provider_name));
    }
  }
  $batch = array(
    'operations' => $operations,
    'finished' => 'domains_domain_inport_process_finished',
  );
  
  batch_set($batch);
  // Delete cache
  ctools_object_cache_clear('import-domain', 'import-domain');
  $form_state['redirect'] = 'domain';
  return;
}
