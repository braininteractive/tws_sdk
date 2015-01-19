package com.threedsystems.tws.Bucket;

import java.util.HashMap;

public class PresignedUploadForm {
	
	private String upload_id;
	private String bucket;
	private String form_action;
	private String form_method;
	private String form_enctype;
	private HashMap<String, Object> form_fields;
		
	public HashMap<String, Object> getForm_fields() {
		return form_fields;
	}

	public void setForm_fields(HashMap<String, Object> form_fields) {
		this.form_fields = form_fields;
	}

	public String getUpload_id() {
		return upload_id;
	}

	public void setUpload_id(String upload_id) {
		this.upload_id = upload_id;
	}

	public String getBucket() {
		return bucket;
	}

	public void setBucket(String bucket) {
		this.bucket = bucket;
	}

	public String getForm_action() {
		return form_action;
	}

	public void setForm_action(String form_action) {
		this.form_action = form_action;
	}

	public String getForm_method() {
		return form_method;
	}

	public void setForm_method(String form_method) {
		this.form_method = form_method;
	}

	public String getForm_enctype() {
		return form_enctype;
	}

	public void setForm_enctype(String form_enctype) {
		this.form_enctype = form_enctype;
	}
}