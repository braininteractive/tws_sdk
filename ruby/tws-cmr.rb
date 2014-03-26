require 'tws'

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
		m = get_model id

		return '' if m['meta'].nil?
		stov_cmr = m['meta']['stov_cmr']
		return '' if stov_cmr.nil?
		cmr_object = JSON.parse(stov_cmr)

		traverse_cmr cmr_object do |obj|
			next unless obj.class == Hash
			class_type = obj['class_type']
			next if class_type.nil? || class_type != 'StovChunk'
			stor_id = obj['stor_id']
			next if stor_id.nil?
			chunk = get_model stor_id
			next if chunk['meta'].nil?
			link = @links[stor_id]
			link = get_link stor_id, chunk['meta']['filename'] if link.nil?
			@links[stor_id] = link
			obj['url'] = link
			puts link
		end

		return cmr_object
	end
end
