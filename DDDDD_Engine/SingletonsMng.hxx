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

// =-=-= �}�N����`�� =-=-=
#define SINGLETON_MAKES(Type) Type(const Type&) = delete;Type& operator=(const Type&) = delete;Type(Type&&) = delete;Type& operator=(Type&&) = delete;static Type& GetInstance(){return Singleton<Type>::GetInstance();} 

// =-=-= �萔��`�� =-=-=
enum class UPDATE_ORDER {
	NO_UPDATE = -1,	//�X�V���Ȃ�
	FIRST_UPDATE,	//�P�ԂɍX�V
	SECOND_UPDATE,	//�Q�ԂɍX�V
	THIRD_UPDATE,	//�R�ԂɍX�V
	LAST_UPDATE		//�Ō�ɍX�V
};

/// @brief �V���O���g���̊��N���X
class SingletonBase
{
public:
	/// @brief �R���X�g���N�^���֎~
	SingletonBase() = delete;
	/// @brief �R���X�g���N�^
	SingletonBase(UPDATE_ORDER Order);
	
	/// @brief ���������� ���I�[�o�[���C�h���Ă�������
	virtual bool Init() = 0;
	/// @brief �X�V���� ���I�[�o�[���C�h���Ă�������
	virtual void Update() {};
	/// @brief �f�X�g���N�^
	virtual ~SingletonBase() {};
};

/// @brief �V���O���g���̍ŏI�������s���N���X
class Supervision final
{
	template<typename T>
	friend class Singleton;
	friend class SingletonBase;
public:
	/// @brief �������������s��
	/// @return ����������true
	static bool Initialize();
	/// @brief �X�V�������s��
	static void Updater();
	/// @brief �I���������s��
	static void Finalize();

private:
	/// @brief �I��������ǉ�����
	/// @param func �I������
	static void addFinalizer(void(*func)());
	/// @brief �X�V������ǉ�����
	/// @param pSingleton ��������V���O���g��
	/// @param order �X�V��
	static void addUpdater(SingletonBase* pSingleton, UPDATE_ORDER order);

private:
	static std::array<std::vector<SingletonBase*>, static_cast<int>(UPDATE_ORDER::LAST_UPDATE) + 1> m_Updaters;//�X�V����
	static std::vector<void(*)()> m_finalizers;//�I������
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
		return *instance;
	}
private:
	/// @brief �C���X�^���X�𐶐�����
	static void Create()
	{
		instance = new Type;
		Supervision::addFinalizer(&Singleton<Type>::destroy);
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