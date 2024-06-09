#include "SingletonsMng.hxx"

std::array<std::vector<SingletonBase*>, 3> SingletonManager::m_Updaters;//�X�V����
std::vector<SingletonManager::Function> SingletonManager::m_finalizers;//�I������

namespace 
{
	std::mutex gMutex;// �r������p
}

/// @brief �R���X�g���N�^
SingletonBase::SingletonBase()
{
	SingletonManager::addUpdater(this, m_Order);// �X�V������ǉ�
}

/// @brief �I��������ۑ�
/// @param func �I������
void SingletonManager::addFinalizer(Function func)
{
	std::lock_guard<std::mutex> lock(gMutex);// �r������
	m_finalizers.push_back(func);// �I��������ǉ�
}

/// @brief �X�V������ۑ�
/// @param pSingleton ��������V���O���g��
/// @param order �X�V��
void SingletonManager::addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order)
{
	if(order == UPDATE_ORDER::NO_UPDATE)// �X�V���Ȃ��ꍇ�͒ǉ����Ȃ�
		return;
	m_Updaters[static_cast<int>(order) - 1].push_back(pSingleton);// �X�V������ǉ�
}

/// @brief �X�V�������s��
void SingletonManager::Updater()
{
	for (auto& updaters : m_Updaters)
		for (auto& updater : updaters)
			updater->Update();// �X�V���������s
}

/// @brief �I���������s��
void SingletonManager::finalize()
{
	std::lock_guard<std::mutex> lock(gMutex);// �r������
	for (auto& elem : m_Updaters)// �X�V�������N���A
		elem.clear();
	for (auto& elem : m_finalizers)// �I�����������s
		(*elem)();
}