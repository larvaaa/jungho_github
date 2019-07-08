package org.zerock.service;

import java.util.List;

import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import org.zerock.domain.BoardAttachVO;
import org.zerock.domain.BoardVO;
import org.zerock.domain.Criteria;
import org.zerock.mapper.BoardAttachMapper;
import org.zerock.mapper.BoardMapper;

@Service
public class BoardServiceImpl implements BoardService {

	private BoardMapper mapper;
	private BoardAttachMapper attachMapper;
	
	public BoardServiceImpl(BoardMapper mapper, BoardAttachMapper attachMapper) {
		this.mapper = mapper;
		this.attachMapper = attachMapper;
	}

	public BoardMapper getMapper() {
		return mapper;
	}

	public void setMapper(BoardMapper mapper) {
		this.mapper = mapper;
	}

	public BoardAttachMapper getAttachMapper() {
		return attachMapper;
	}

	public void setAttachMapper(BoardAttachMapper attachMapper) {
		this.attachMapper = attachMapper;
	}

	@Transactional
	@Override
	public void register(BoardVO board) {
		mapper.insertSelectKey(board);
		if (board.getAttachList() == null || board.getAttachList().size() <= 0) {
			return;
		}
		board.getAttachList().forEach(attach -> {
			attach.setBno(board.getBno());
			attachMapper.insert(attach);
		});
	}

	@Override
	public BoardVO get(Long bno) {
		return mapper.read(bno);
	}

	@Transactional
	@Override
	public boolean modify(BoardVO board) {
		attachMapper.deleteAll(board.getBno());
		boolean modifyResult = mapper.update(board) == 1;
		if (modifyResult && board.getAttachList().size() > 0) {
			board.getAttachList().forEach(attach -> {
				attach.setBno(board.getBno());
				attachMapper.insert(attach);
			});
		}
		return modifyResult;
	}

	@Transactional
	@Override
	public boolean remove(Long bno) {
		attachMapper.deleteAll(bno);
		return mapper.delete(bno) == 1;
	}

	@Override
	public List<BoardVO> getList(Criteria cri) {
		return mapper.getListWithPaging(cri);
	}

	@Override
	public int getTotal(Criteria cri) {
		return mapper.getTotalCount(cri);
	}

	@Override
	public List<BoardAttachVO> getAttachList(Long bno) {
		return attachMapper.findByBno(bno);
	}

	@Override
	public void removeAttach(Long bno) {
		attachMapper.deleteAll(bno);
	}

}
