package org.zerock.security;

import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.zerock.domain.MemberVO;
import org.zerock.mapper.MemberMapper;
import org.zerock.security.domain.CustomUser;

import lombok.extern.log4j.Log4j;

@Log4j
public class CustomUserDetailsService implements UserDetailsService {


	private MemberMapper memberMapper;
	
	
	public CustomUserDetailsService(MemberMapper memberMapper) {
		super();
		this.memberMapper = memberMapper;
	}

	public MemberMapper getMemberMapper() {
		return memberMapper;
	}

	public void setMemberMapper(MemberMapper memberMapper) {
		this.memberMapper = memberMapper;
	}


	@Override
	public UserDetails loadUserByUsername(String userName) throws UsernameNotFoundException {

	

		// userName means userid
		MemberVO vo = memberMapper.read(userName);

		return vo == null ? null : new CustomUser(vo);
	} 

}
