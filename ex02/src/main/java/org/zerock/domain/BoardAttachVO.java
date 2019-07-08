package org.zerock.domain;

public class BoardAttachVO {

  private String uuid;
  private String uploadPath;
  private String fileName;
  private boolean fileType;
  
  private Long bno;

public String getUuid() {
	return uuid;
}

public void setUuid(String uuid) {
	this.uuid = uuid;
}

public String getUploadPath() {
	return uploadPath;
}

public void setUploadPath(String uploadPath) {
	this.uploadPath = uploadPath;
}

public String getFileName() {
	return fileName;
}

public void setFileName(String fileName) {
	this.fileName = fileName;
}

public boolean isFileType() {
	return fileType;
}

public void setFileType(boolean fileType) {
	this.fileType = fileType;
}

public Long getBno() {
	return bno;
}

public void setBno(Long bno) {
	this.bno = bno;
}
  
  
}
