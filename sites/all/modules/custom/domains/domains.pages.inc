<?php

/**
 * @file
 * Function to domains.module pages.
 */

/**
 * Domain dashboard menu callback.
 */
function domains_dashboard_page() {
  global $user;

  $results = domains_domain_load_multiple(FALSE, array('uid' => $user->uid));
  $domains = array();

  foreach ($results as $domain) {
    $domains[] = l($domain->name, "domain/" . $domain->id);
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
  ctools_include('object-cache');
  if ($cache = ctools_object_cache_get('domains', 'register-domains-selected')) {
    if (!isset($form_state['values']['domain']) && isset($cache->domain)) {
      $form_state['values']['domain'] = $cache->domain;
    }
    if (!isset($form_state['values']['domains']) && isset($cache->domains)) {
      $form_state['values']['domains'] = $cache->domains;
    }
  }

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
    '#default_value' => !empty($form_state['values']['domain']) ? $form_state['values']['domain'] : NULL,
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
  if (!empty($check_result)) {
    $has_avai = FALSE;
    
    $form['results']['domains'] = array(
      '#type'   => 'checkboxes',
      '#check_result' => $check_result,
      '#title'  => t('Available Domains'),
      '#options' => array(),
      '#default_value' => !empty($form_state['values']['domains']) ? $form_state['values']['domains'] : array(),
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

      if (user_is_anonymous()) {
        $form['results']['purchase']['#value'] = t('Register or login to purchase');
      }
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
  
  if (!user_is_anonymous()) {
    if (!empty($selected_domains)) {
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
  }
  else {
    ctools_include('object-cache');
    ctools_include('ajax');
    
    # Cache form values
    $register = new stdClass();
    $register->domain  = $form_state['values']['domain'];
    $register->domains = $form_state['values']['domains'];
    ctools_object_cache_set('domains', 'register-domains-selected', $register);

    # Redirect user to login form
    $commands[] = ctools_ajax_command_redirect('user/login', 0, array('query' => array("destination=domain/register")));
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