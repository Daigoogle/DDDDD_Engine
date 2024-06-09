//	�t�@�C����	�FSingletonsMng.hxx
//	  �T  �v		�FMozc�����x�[�X�Ƃ����V���O���g�����Ǘ�����N���X
//	��	��	��	�Fdaigo
//	 �쐬����	�F2024/06/09 20:37:20
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

// =-=-= �C���N���[�h�K�[�h�� =-=-=
#ifndef _____SingletonsMng_HXX_____
#define _____SingletonsMng_HXX_____

// =-=-= �C���N���[�h�� =-=-=
#include <array>
#include <vector>
#include <mutex>

// =-=-= �萔��`�� =-=-=
enum class UPDATE_ORDER {
	NO_UPDATE = 0,	//�X�V���Ȃ�
	FIRST_UPDATE,	//�ŏ��ɍX�V
	MIDDLE_UPDATE,	//���ԂɍX�V
	LAST_UPDATE,	//�Ō�ɍX�V
};

/// @brief �V���O���g���̊��N���X
class SingletonBase
{
public:
	/// @brief �R���X�g���N�^
	SingletonBase();

	/// @brief ���������� ���I�[�o�[���C�h���Ă�������
	virtual void Init() = 0;

	/// @brief �X�V���� ���K�v�ȏꍇ�̂݃I�[�o�[���C�h���Ă�������
	virtual void Update() {};

	/// @brief �f�X�g���N�^ ���I�[�o�[���C�h���Ă�������
	virtual ~SingletonBase() = 0;
private:
	UPDATE_ORDER m_Order = UPDATE_ORDER::NO_UPDATE;//�X�V�� ���C�j�V�����C�U�[�ŔC�ӂ̒l�ɏ��������Ă�������
};

/// @brief �V���O���g���̍ŏI�������s���N���X
class SingletonManager final
{
public:
	/// @brief �I�������̊֐��^
	using Function = void(*)();

	/// @brief �I��������ǉ�����
	/// @param func �I������
	static void addFinalizer(Function func);

	/// @brief �X�V������ǉ�����
	/// @param pSingleton ��������V���O���g��
	/// @param order �X�V��
	static void addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order);

	/// @brief �X�V�������s��
	static void Updater();

	/// @brief �I���������s��
	static void finalize();

private:
	static std::array<std::vector<SingletonBase*>, 3> m_Updaters;//�X�V����
	static std::vector<Function> m_finalizers;//�I������
};

/// @brief �V���O���g���̍ŏI�������s���N���X
/// @tparam Type �V���O���g���̌^
template<typename Type>
class Singleton final
{
public:
	/// @brief �C���X�^���X���擾����
	/// @return �C���X�^���X
	static Type& GetInstance()
	{
		//���߂ČĂяo���ꂽ�Ȃ�C���X�^���X�̐���
		std::call_once(initFlag, Create);
		assert(instance);
		return *instance;
	}

	/// @brief �C���X�^���X���擾����
	explicit operator Type&()
	{
		return GetInstance();
	}
private:
	/// @brief �C���X�^���X�𐶐�����
	static void Create()
	{
		static_cast<SingletonBase>(Type);
		instance = new Type;
		SingletonFinalizer::addFinalizer(&Singleton<Type>::destroy);
	}

	/// @brief �C���X�^���X��j������
	static void destroy()
	{
		delete instance;
		instance = nullptr;
	}

	static std::once_flag initFlag;	//��������̃t���O(���񏈗��Ή�)
	static Type* instance;			//����̃C���X�^���X
};

//	�ÓI�����o���`
template <typename Type> std::once_flag Singleton<Type>::initFlag;
template <typename Type> Type* Singleton<Type>::instance = nullptr;

#endif // !_____SingletonsMng_HXX_____