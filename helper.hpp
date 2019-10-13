



#ifndef UTILI_GDL_HPP
#define UTILI_GDL_HPP



namespace gdl {




	struct noncopyable {
		noncopyable() = default;
		virtual ~noncopyable() = default;
		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable&) = delete;
	};











}






#endif