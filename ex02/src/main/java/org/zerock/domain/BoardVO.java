package org.zerock.domain;

import java.util.Date;
import java.util.List;


public class BoardVO {

	private Long bno;
	private String title;
	private String content;
	private String writer;
	private Date regdate;
	private Date updateDate;
	private int replyCnt;
	private List<BoardAttachVO> attachList;
	
	/*public BoardVO(Long bno, String title, String content,String writer, Date regdate,Date updateDate,int replyCnt,List<BoardAttachVO> attachList) {
		this.bno = bno;
		this.title = title;
		this.content = content;
		this.writer = writer;
		this.regdate = regdate;
		this.updateDate = updateDate;
		this.replyCnt = replyCnt;
		this.attachList = attachList;
	}*/
	
	public Long getBno() {
		return bno;
	}

	public void setBno(Long bno) {
		this.bno = bno;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getContent() {
		return content;
	}

	public void setContent(String content) {
		this.content = content;
	}

	public String getWriter() {
		return writer;
	}

	public void setWriter(String writer) {
		this.writer = writer;
	}

	public Date getRegdate() {
		return regdate;
	}

	public void setRegdate(Date regdate) {
		this.regdate = regdate;
	}

	public Date getUpdateDate() {
		return updateDate;
	}

	public void setUpdateDate(Date updateDate) {
		this.updateDate = updateDate;
	}

	public int getReplyCnt() {
		return replyCnt;
	}

	public void setReplyCnt(int replyCnt) {
		this.replyCnt = replyCnt;
	}

	public List<BoardAttachVO> getAttachList() {
		return attachList;
	}

	public void setAttachList(List<BoardAttachVO> attachList) {
		this.attachList = attachList;
	}
	
	
}
