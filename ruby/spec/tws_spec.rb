# This test suite is targeted for acceptance testing for staging env.
# Test would be slow becuase all tests are ran on running staging apps.
# The first run can be  very slow due to staging app's warming up time.
# TWS_API_KEY and TWS_API_SECRET should be set for staging server accounts.
# Usage: rspec tws_spec.rb

load '../tws.rb'

describe TWS do

  before(:all) do
    @tws = TWS.new  :api_key => ENV['TWS_STAGING_KEY'],
                    :api_secret => ENV['TWS_STAGING_SECRET'],
                    :stid_host => "https://stid-staging.herokuapp.com",
                    :stor_host => "https://stor-staging.herokuapp.com",
                    :stom_host => "https://stom-staging.herokuapp.com",
                    :stopp_host => "https://stopp-staging.herokuapp.com"
  end
  
  describe "api key and secret" do
    it "need to be set" do
      ENV['TWS_STAGING_KEY'].should_not be_blank
      ENV['TWS_STAGING_SECRET'].should_not be_blank
    end
  end
  
  describe "authenticate" do
    it "authenticates " do
      @tws.authenticate["authenticated"].should == true
    end
  end
  
  describe "STOR" do
    before(:all) do
      @m = @tws.create_model(:name => "from rspec")
    end
    
    it "creates a model" do
      @m["meta"]["name"].should == "from rspec"
    end
    
    it "gets a model" do
      @tws.get_model(@m["id"])["id"].should == @m["id"]
    end
    
    it "gets models" do
      @tws.get_models.should_not be_blank
    end
    
    it "updates the model" do
      @tws.update_model(@m["id"], :name => "updated from rspec")["meta"]["name"].should == "updated from rspec"
    end
    
    it "presigns a upload form" do
      @tws.presigned_upload_form["upload_id"].should_not be_blank
    end
    
    describe "uploads a model" do
      before(:all) do
        @uploaded_model = @tws.upload_model("tws_spec.rb", :meta => {:filename => 'tws_spec.rb', :filesize => File.size('tws_spec.rb')})
      end
      
      it "uploads the model" do
        @uploaded_model["meta"]["filename"].should == "tws_spec.rb"
      end
      
      it "gets a link to download the model" do
        link = @tws.get_link(@uploaded_model["id"])
        link.should include(@uploaded_model["id"])
        RestClient.get(link).headers[:content_length].should == @uploaded_model["meta"]["filesize"]
      end
      
      it "deletes a model" do
        @tws.delete_model(@uploaded_model["id"]).should == 204
      end
    end
    
    it "deletes a model" do
      @tws.delete_model(@m["id"]).should == 204
    end
    
  end
  
  describe "STOM" do
    before(:all) do
      @s = @tws.create_session 300, "latest"
    end
    
    it "creates a session" do
      @s['engine_version'].should == 'latest'
    end
    
    it "gets a session" do
      @tws.get_session(@s["id"])["id"].should == @s["id"]
    end
    
    it "gets sessions" do
      @tws.get_sessions.first["id"] == @s["id"]
    end
    
    describe "Run with blender" do
      before(:all) do
        @run = @tws.create_run(@s["id"], "blender", "print('3WS rocks hard!')")
        sleep(3)
      end
      
      it "runs a code" do
        @run["id"].should_not be_blank
      end
    
      it "gets a run" do
        @tws.get_run(@s["id"], @run["id"])["result"].should include("3WS rocks hard!")
      end
      
      it "gets runs" do
        @tws.get_runs(@s["id"]).should_not be_empty
      end
    end

    def run_a_code(code_fn, seconds=5)
      @run = @tws.create_run(@s["id"], @default_platform, File.read(code_fn))
      sleep(seconds)
      return JSON.parse(@tws.get_run(@s["id"], @run["id"])["result"])
    end

    describe "Test validator" do
      def is_valid_results?(results, num_units, num_processes)
        return false unless results.kind_of?(Array) and results.length == num_units
        results.each do |result|
          processes = result['validation']
          return false unless processes.length == num_processes
          processes.each { |process| return false unless process.has_key?('valid') }
        end
        return true
      end

      before(:all) do
        @default_platform = 'blender'
      end

      it "runs with single unit and process" do
        results = run_a_code('codes/validator.unit_x1_and_pp_x1.py', 5)
        is_valid_results?(results, 1, 1).should be_true
      end

      it "runs with multiple units and processes" do
        results = run_a_code('codes/validator.unit_x3_and_pp_x3.py', 10)
        is_valid_results?(results, 3, 3).should be_true
      end
    end
  end

  describe "STOPP" do
    it "gets printers" do
      @tws.get_printers.should_not be_blank
    end
    
    it "gets materials" do
      @tws.get_materials.should_not be_blank
    end
  end

end