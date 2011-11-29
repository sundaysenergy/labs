<?php
ignore_user_abort(1);

define('GIT_BRANCH', 'master');
define('UNF_SUBDOMAIN', 'sundaysenergy');
define('UNF_PID', 37323); // Project ID
define('UNF_RID', 26); // Repository ID
define('CAMP_SUBDOMAIN', 'sundaysenergy');
define('CAMP_TOKEN', 'a3241be299ce1b45b2d5c1d2cb0752abed896ee0');
define('CAMP_PW', 'cc2de8a01a7c5d662');
//To get room_id, login to your CF room and look in the address bar (after /room)
define('CAMP_RM', 435558); // Room ID

$master_server = 'www.acfdev.com';
$other_servers = array();

$file_name = '/git-post-commit.php';
$request = $HTTP_RAW_POST_DATA;
$paste = array();

$revision = $author = $message = $repo = 'n/a';
if ($request) {
  if ($master_server == $_SERVER['SERVER_NAME'] && !empty($other_servers)) {
    foreach ($other_servers as $server_name) {
      $url = $server_name . $file_name;
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-Type: text/xml"));
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, "$request");
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
      $paste[$server_name] = curl_exec($ch);
      curl_close($ch);
    }
  }
  $reader = new XMLReader();
  $reader->XML($request);

  while ($reader->read()) {
    if ($reader->name == "revision" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $revision = $reader->value;
    }
    
    if ($reader->name == "author-name" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $author = $reader->value;
    }
    
    if ($reader->name == "message" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $message = $reader->value;
    }
    if ($reader->name == "id" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $repo = $reader->value;
    }
  }
  $reader->close();
}

$git = 'pull ' . GIT_BRANCH;
$paste[$git] = exec('/usr/bin/git pull');
if (!$paste['pull master'] || !is_string($paste[$git])) {
  $paste['pull master'] = 'NO GIT Response';
}
//$paste['globals'] = print_r($GLOBALS, true);
if ($author && is_string($author)) {
  $paste['author'] = $author;
}
if ($revision && is_string($revision)) {
  $paste['revision'] = $revision;
}
if ($message && is_string($message)) {
  $paste['message'] = $message;
}

$url = 'https://' . UNF_SUBDOMAIN . 'campfirenow.com';	// Use full URL, http://[account].campfirenow.com
$repo_l = ' https://sundaysenergy.unfuddle.com/a#/projects/' . UNF_PID . '/repositories/' . UNF_RID . '/commit?commit=' . $revision;

$icecube = new icecube($url, CAMP_TOKEN);
$icecube->login();
//$icecube->joinRoom($room_id);
//$icecube->leaveRoom($room_id);
$say = $author . " commit: '" . $message . "'";
$say2 = ' Site status: ' . $paste['pull master'];
$icecube->speak($say . $repo_l . $say2, CAMP_RM);
// $icecube->speak($say2, $room_acf);
// $icecube->speak(print_r($paste, TRUE), $room_dev, TRUE);
// $icecube->speak($say . $repo_l . $say2, $room_se);
// $icecube->speak($say2, $room_se);

$icecube->logout();

$fh = fopen("gitpull.txt", 'a') or die("can't open file");
fwrite($fh, print_r($paste, TRUE));
fclose($fh);

echo $paste['pull master'];
/**
 * Ice Cube - Campfire API for PHP
 * 
 * @author      Emil Sundberg (emil@mimmin.com) at mimmin (www.mimmin.com)
 * Ported to the new campfire API by Tom Boutell (tom@punkave.com) at P'unk Avnue (punkave.com)
 * 
 * Copyright 2009, Emil Sundberg, Mimmin AB, www.mimmin.com
 *
 * Licensed under The MIT License
 * Redistributions of files must retain the above copyright notice.
 *
 * @copyright   Copyright 2009, Emil Sundberg, Mimmin AB, www.mimmin.com
 * @link      http://labs.mimmin.com/icecube
 * @version     0.4
 * @license     http://www.opensource.org/licenses/mit-license.php The MIT License
 * 
 * Versions
 * -----------
 * 2008-01-22 0.1   First release
 * 2009-04-04 0.2   Verbose option added, thanks to Tom Boutell (tom@punkave.com)
 * 2009-11-01 0.3   Avoiding CSRF protection, own useragent and tries to mimic the official client better. 
 *            Changes provided by Tom Boutell (tom@punkave.com)
 * 2009-12-19 0.4   Ported to the new official Campfire API (developer.37signals.com/campfire) with backward compatibility by Tom Boutell
 */
class icecube {
  private $url;
  private $authtoken;
  private $useragent;
  private $cookie;
  private $verbose;

  /*  
    The password is now ignored, and in place of the username you
    should pass the authentication token. Go to "edit my 
    Campfire account" in Campfire and click "Reveal authentication
    token for API" to get your authentication token. I've kept
    the password field to make this backwards compatible with
    existing applications of this class. tom@punkave.com 
  */
  public function __construct($url, $authtoken, $password = false, $verbose = false) {
    $this->url = $url;
    $this->authtoken = $authtoken;
  
    // We don't care about $password anymore
  
    $this->verbose = $verbose;
    // tom@punkave.com: Tinder has its own user agent string, so 
    // there's no need for us to fake IE either... help 37signals 
    // possibly help us by letting them know we're out there
    $this->useragent = 'IceCube/0.4 (http://labs.mimmin.com/icecube/)';
    $this->cookie = "mycookie";

  }
  
  public function login() {
    // The new official API is stateless, there is no
    // concept of login
    return;
  }
  
  public function joinRoom($room_id) {
    $this->_getPageByPost('/room/'.$room_id.'/join.json');
  }

  public function leaveRoom($room_id) {
    $this->_getPageByPost('/room/'.$room_id.'/leave.json');
  }
  
  public function logout() {
    // New API is stateless, no concept of logout
  }
  
  public function speak($message, $room_id, $is_paste = false) {
    $page = '/room/'.$room_id.'/speak.json';
    if ($is_paste) {
      $type = 'PasteMessage';
    }
    else {
      $type = 'TextMessage';
    }
    return $this->_getPageByPost($page,
      array('message' => array('type' => $type, 'body' => $message)));
  }
  
  private function _getPageByPost($page, $data = null) {
    $url = $this->url . $page;
    // The new API allows JSON, so we can pass 
    // PHP data structures instead of old school POST
    $json = json_encode($data);
  
    // cURL init & config
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_POST,1);
    curl_setopt($ch, CURLOPT_USERAGENT, $this->useragent);
    curl_setopt($ch, CURLOPT_VERBOSE, $this->verbose); 
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_USERPWD, $this->authtoken . ':x'); 
    curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-type: application/json"));
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie); 
    
    curl_setopt($ch, CURLOPT_POSTFIELDS,$json);
    $output = curl_exec($ch);
    
    curl_close($ch);
    
    // We tend to get one space with an otherwise blank response
    $output = trim($output);  

    if (strlen($output)) { 
      /* Responses are JSON. Decode it to a data structure */
      return json_decode($output);
    }
    // Simple 200 OK response (such as for joining a room)
    // TODO: check for other result codes here
    return true;
  }
  
  private function _getPageByGet($page = '') {
    $url = $this->url . $page;
    empty($referer) ? $referer = $this->url : $referer = $this->url . $referer;
    
    // cURL init & config
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_USERAGENT, $this->useragent);
    curl_setopt($ch, CURLOPT_VERBOSE, $this->verbose); 
    curl_setopt($ch, CURLOPT_REFERER, $referer);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_COOKIEFILE, $this->cookie); 
    curl_setopt($ch, CURLOPT_USERPWD, $this->authtoken . ':x'); 
    $output = curl_exec($ch);
    
    curl_close($ch);
    
    /* Responses are XML. THis returns a SimpleXMLElement */
    return simplexml_load_string($output);
  }
}

?>
