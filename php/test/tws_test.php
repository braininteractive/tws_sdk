<?php

class test extends PHPUnit_Framework_TestCase
{
	protected $tws;
	
	public function __construct(){
		$this->tws = new Tws(array("api_key"=>$_ENV["TWS_STAGING_KEY"], "api_secret"=>$_ENV["TWS_STAGING_SECRET"]));
		$this->tws->stid_host = "https://stid-staging.herokuapp.com";
		$this->tws->stom_host = "https://stom-staging.herokuapp.com";
		$this->tws->stor_host = "https://stor-staging.herokuapp.com";	
	}

	public function testAuthenticate()
	{
		$response = json_decode($this->tws->authenticate(), 1);
		$this->assertEquals(true, $response["authenticated"]);
	}

	public function testCreateSession()
	{
		$response = json_decode($this->tws->create_session(), 1);
		$this->assertEquals("latest", $response["engine_version"]);
	}

}
?>