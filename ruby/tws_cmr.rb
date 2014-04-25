require_relative 'tws' rescue nil

CHUNK_EXTENSION = ".tws"

class TWS_CMR < TWS
	def initialize opts={}
		super opts
		@links = Hash.new
	end
	
	def traverse_cmr(object, *args, &block)
		return if object.nil?
		block.call(object, args)
		
		if object.class == Array
			object.each {|x| traverse_cmr(x, args, &block) }
		elsif object.class == Hash
			object.each {|k, v| traverse_cmr(v, args, &block)}
		end
	end
	private :traverse_cmr

	def get_CMR id
#		get_link_time = 0.0
#		start = Time.now
		
		m = get_model id

#		puts "get_model: " + (Time.now - start).to_s
		
		return '' if m['meta'].nil?
		stov_cmr = m['meta']['stov_cmr']
		return '' if stov_cmr.nil? || stov_cmr.empty?
		
		begin
			cmr_object = JSON.parse(stov_cmr)
		rescue
			return ''
		end
		
		traverse_cmr cmr_object do |obj|
			next unless obj.class == Hash
			class_type = obj['class_type']
			next if class_type.nil? || class_type != 'StovChunk'
			stov_id = obj['id']
			next if stov_id.nil?
			stor_id = obj['stor_id']
			next if stor_id.nil?
	#		chunk = get_model stor_id
	#		next if chunk['meta'].nil?
			link = @links[stor_id]
	#		get_link_start = Time.now
	#		link = get_link stor_id, chunk['meta']['filename'] if link.nil?
	#		get_link_time += Time.now - get_link_start
			if link.nil?
				filename = obj['filename']
				filename = stov_id + CHUNK_EXTENSION if filename.nil?
				link = get_link stor_id, filename 
			end
			@links[stor_id] = link
			obj['url'] = link
			#puts link
		end

	#	puts "total: " + (Time.now - start).to_s
	#	puts "get_link: " + get_link_time.to_s
		return cmr_object
	end
end
