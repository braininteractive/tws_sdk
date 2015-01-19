package tws_sdk_test;

import com.threedsystems.tws.*;

public class TwsMain {

	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		StorClient stor = new StorClient();
		System.out.println(stor.GetSDKVersion());
		
		System.out.println(stor.UploadModel("/Users/inska/Downloads/test.jpg"));
		
	}
}
