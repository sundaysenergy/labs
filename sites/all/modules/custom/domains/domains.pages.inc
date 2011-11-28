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

  $results = domain_load_multiple(FALSE, array('uid' => $user->uid));
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