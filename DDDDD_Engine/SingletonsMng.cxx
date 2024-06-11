#include "SingletonsMng.hxx"

namespace 
{
	std::array<std::vector<SingletonBase*>, static_cast<int>(UPDATE_ORDER::LAST_UPDATE) + 1> g_Updaters;//�X�V����
	std::vector<void(*)()> g_finalizers;//�I������
	std::mutex gMutex;// �r������p
}

/// @brief �R���X�g���N�^
SingletonBase::SingletonBase(UPDATE_ORDER Order)
{
	Supervision::addUpdater(this, Order);// �X�V������ǉ�
}

/// @brief �I��������ۑ�
/// @param func �I������
void Supervision::addFinalizer(void(*func)())
{
	std::lock_guard<std::mutex> lock(gMutex);// �r������
	g_finalizers.push_back(func);// �I��������ǉ�
}

/// @brief �X�V������ۑ�
/// @param pSingleton ��������V���O���g��
/// @param order �X�V��
void Supervision::addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order)
{
	if(order == UPDATE_ORDER::NO_UPDATE)// �X�V���Ȃ��ꍇ�͒ǉ����Ȃ�
		return;
	g_Updaters[static_cast<int>(order)].push_back(pSingleton);// �X�V������ǉ�
}

/// @brief �X�V�������s��
void Supervision::Updater()
{
	for (auto& updaters : g_Updaters)
		for (auto& updater : updaters)
			updater->Update();// �X�V���������s
}

/// @brief �I���������s��
void Supervision::Finalize()
{
	std::lock_guard<std::mutex> lock(gMutex);// �r������
	for (auto& elem : g_Updaters)// �X�V�������N���A
		elem.clear();
	for (int i = g_finalizers.size() - 1; i >= 0;i--)// �I���������t���Ɏ��s
		g_finalizers[i]();
}